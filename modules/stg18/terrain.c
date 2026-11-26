#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg18/dry_lagoon.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char dry_lagoon_land_filenames[583] = {
#include "stg18/terrain/dry_lagoon_land_filenames.inc"
};

/* STG18.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between dry_lagoon.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[217];
struct land_table dry_lagoon_land;
static NJS_TEXNAME textures[35];
NJS_TEXLIST dry_lagoon_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[19];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[5];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[4];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[5];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[4];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[5];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[4];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[19];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[8];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[41];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[20];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[19];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[8];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[52];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[24];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[19];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[12];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[5];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[4];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[5];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[4];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[5];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[4];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[5];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[4];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[558];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[237];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[153];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[57];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[82];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[37];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[78];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[31];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[95];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[42];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[67];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[29];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[144];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[64];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[141];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[64];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[190];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[74];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[30];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[18];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[30];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[18];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[30];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[18];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[30];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[18];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[42];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[12];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[18];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[12];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[443];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[155];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[220];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[93];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[97];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[42];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[97];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[42];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[103];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[42];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[15];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[12];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[30];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[24];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[416];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[167];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[96];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[41];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[42];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[20];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[116];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[44];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[5];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[4];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[102];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[48];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[30];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[18];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[30];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[18];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[30];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[18];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[30];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[18];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[35];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[16];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[35];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[16];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[215];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[55];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[221];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[56];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[208];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[55];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[219];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[56];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[43];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[17];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[28];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[12];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[28];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[12];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[28];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[12];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[52];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[24];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[72];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[30];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[22];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[12];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[25];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[12];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[40];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[18];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[37];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[18];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[62];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[24];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[48];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[18];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[48];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[18];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[48];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[18];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[48];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[18];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[48];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[18];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[16];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[8];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[16];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[8];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[15];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[8];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[21];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[9];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[243];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[64];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[10];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[6];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[50];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[24];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[12];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[7];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[17];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[9];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static Sint16 draw_139_polygons[111];
static Sint32 draw_139_vertices[103];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_138_polygons[25];
static Sint32 draw_138_vertices[19];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[79];
static Sint32 draw_137_vertices[43];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[91];
static Sint32 draw_136_vertices[51];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[91];
static Sint32 draw_135_vertices[51];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[183];
static Sint32 draw_134_vertices[103];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[365];
static Sint32 draw_133_vertices[223];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[25];
static Sint32 draw_132_vertices[19];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[37];
static Sint32 draw_131_vertices[35];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_130_polygons[71];
static Sint32 draw_130_vertices[51];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[71];
static Sint32 draw_129_vertices[51];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[79];
static Sint32 draw_128_vertices[59];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[479];
static Sint32 draw_127_vertices[303];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[25];
static Sint32 draw_126_vertices[19];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[85];
static Sint32 draw_125_vertices[59];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[487];
static Sint32 draw_124_vertices[311];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[25];
static Sint32 draw_123_vertices[19];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[79];
static Sint32 draw_122_vertices[59];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[79];
static Sint32 draw_121_vertices[59];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[215];
static Sint32 draw_120_vertices[151];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_119_polygons[215];
static Sint32 draw_119_vertices[151];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_118_polygons[39];
static Sint32 draw_118_vertices[35];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_117_polygons[39];
static Sint32 draw_117_vertices[35];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_116_polygons[73];
static Sint32 draw_116_vertices[51];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[71];
static Sint32 draw_115_vertices[51];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[43];
static Sint32 draw_114_vertices[27];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_113_polygons[43];
static Sint32 draw_113_vertices[27];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[43];
static Sint32 draw_112_vertices[27];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_111_polygons[157];
static Sint32 draw_111_vertices[123];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_110_polygons[211];
static Sint32 draw_110_vertices[139];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_108_polygons[51];
static Sint32 draw_108_vertices[35];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_107_polygons[183];
static Sint32 draw_107_vertices[127];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_106_polygons[425];
static Sint32 draw_106_vertices[251];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[125];
static Sint32 draw_105_vertices[75];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[51];
static Sint32 draw_104_vertices[43];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[95];
static Sint32 draw_103_vertices[63];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[51];
static Sint32 draw_102_vertices[43];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[95];
static Sint32 draw_101_vertices[63];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[51];
static Sint32 draw_100_vertices[43];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[95];
static Sint32 draw_99_vertices[63];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[51];
static Sint32 draw_98_vertices[43];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_97_polygons[95];
static Sint32 draw_97_vertices[63];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[153];
static Sint32 draw_96_vertices[75];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[269];
static Sint32 draw_95_vertices[139];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[267];
static Sint32 draw_94_vertices[139];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[189];
static Sint32 draw_93_vertices[135];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[139];
static Sint32 draw_92_vertices[95];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_91_polygons[75];
static Sint32 draw_91_vertices[59];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_90_polygons[183];
static Sint32 draw_90_vertices[131];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_89_polygons[103];
static Sint32 draw_89_vertices[75];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_88_polygons[103];
static Sint32 draw_88_vertices[75];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_87_polygons[103];
static Sint32 draw_87_vertices[75];
static NJS_CNK_MODEL draw_87_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_86_polygons[103];
static Sint32 draw_86_vertices[75];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_85_polygons[1401];
static Sint32 draw_85_vertices[783];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_84_polygons[1401];
static Sint32 draw_84_vertices[735];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_83_polygons[1383];
static Sint32 draw_83_vertices[819];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_82_polygons[1383];
static Sint32 draw_82_vertices[819];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_82;
static Sint16 draw_81_polygons[141];
static Sint32 draw_81_vertices[91];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[43];
static Sint32 draw_80_vertices[35];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[445];
static Sint32 draw_79_vertices[227];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_78_polygons[445];
static Sint32 draw_78_vertices[227];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_77_polygons[183];
static Sint32 draw_77_vertices[123];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_76_polygons[51];
static Sint32 draw_76_vertices[35];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_75_polygons[201];
static Sint32 draw_75_vertices[171];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_73_polygons[175];
static Sint32 draw_73_vertices[99];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_71_polygons[217];
static Sint32 draw_71_vertices[155];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_70_polygons[287];
static Sint32 draw_70_vertices[151];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[171];
static Sint32 draw_69_vertices[107];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[325];
static Sint32 draw_68_vertices[203];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[297];
static Sint32 draw_67_vertices[167];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[225];
static Sint32 draw_66_vertices[171];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[91];
static Sint32 draw_65_vertices[51];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[89];
static Sint32 draw_64_vertices[51];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[287];
static Sint32 draw_63_vertices[195];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[283];
static Sint32 draw_62_vertices[187];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[143];
static Sint32 draw_61_vertices[95];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[169];
static Sint32 draw_60_vertices[111];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_55_polygons[37];
static Sint32 draw_55_vertices[27];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_54_polygons[329];
static Sint32 draw_54_vertices[175];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[43];
static Sint32 draw_53_vertices[39];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[49];
static Sint32 draw_52_vertices[51];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[45];
static Sint32 draw_51_vertices[35];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[115];
static Sint32 draw_50_vertices[59];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_49_polygons[115];
static Sint32 draw_49_vertices[67];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_48_polygons[135];
static Sint32 draw_48_vertices[95];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_47_polygons[239];
static Sint32 draw_47_vertices[139];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_45_polygons[267];
static Sint32 draw_45_vertices[167];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_44_polygons[219];
static Sint32 draw_44_vertices[151];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[225];
static Sint32 draw_43_vertices[143];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[479];
static Sint32 draw_42_vertices[243];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[479];
static Sint32 draw_41_vertices[243];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[471];
static Sint32 draw_40_vertices[247];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[49];
static Sint32 draw_39_vertices[51];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[95];
static Sint32 draw_38_vertices[71];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[29];
static Sint32 draw_37_vertices[23];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[55];
static Sint32 draw_36_vertices[59];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[321];
static Sint32 draw_35_vertices[239];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[157];
static Sint32 draw_34_vertices[151];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[365];
static Sint32 draw_33_vertices[195];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[1375];
static Sint32 draw_32_vertices[811];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[537];
static Sint32 draw_31_vertices[359];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[215];
static Sint32 draw_30_vertices[175];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[193];
static Sint32 draw_29_vertices[127];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[215];
static Sint32 draw_28_vertices[155];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[215];
static Sint32 draw_27_vertices[175];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[193];
static Sint32 draw_26_vertices[127];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[215];
static Sint32 draw_25_vertices[155];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[141];
static Sint32 draw_24_vertices[79];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[63];
static Sint32 draw_23_vertices[51];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[215];
static Sint32 draw_22_vertices[155];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[193];
static Sint32 draw_21_vertices[131];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[471];
static Sint32 draw_20_vertices[311];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[75];
static Sint32 draw_19_vertices[67];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_17_polygons[107];
static Sint32 draw_17_vertices[83];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_16_polygons[51];
static Sint32 draw_16_vertices[43];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[77];
static Sint32 draw_15_vertices[75];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[37];
static Sint32 draw_14_vertices[35];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[113];
static Sint32 draw_13_vertices[91];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[31];
static Sint32 draw_12_vertices[27];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[131];
static Sint32 draw_11_vertices[83];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[187];
static Sint32 draw_10_vertices[131];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[189];
static Sint32 draw_9_vertices[135];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[141];
static Sint32 draw_8_vertices[99];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[43];
static Sint32 draw_7_vertices[35];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[417];
static Sint32 draw_6_vertices[227];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[221];
static Sint32 draw_5_vertices[131];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[51];
static Sint32 draw_4_vertices[43];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[69];
static Sint32 draw_3_vertices[63];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[63];
static Sint32 draw_2_vertices[59];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[63];
static Sint32 draw_1_vertices[59];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[63];
static Sint32 draw_0_vertices[59];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_55;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_73;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_17;

static struct land_entry entries[217] = {
#include "stg18/terrain/entries.inc"
};
struct land_table dry_lagoon_land = {
#include "stg18/terrain/dry_lagoon_land.inc"
};
static NJS_TEXNAME textures[35] = {
#include "stg18/terrain/textures.inc"
};
NJS_TEXLIST dry_lagoon_land_texlist = {
#include "stg18/terrain/dry_lagoon_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg18/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[19] = {
#include "stg18/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg18/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg18/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg18/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg18/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg18/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[5] = {
#include "stg18/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg18/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[4] = {
#include "stg18/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg18/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg18/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg18/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[5] = {
#include "stg18/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg18/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[4] = {
#include "stg18/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg18/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg18/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg18/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[5] = {
#include "stg18/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg18/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[4] = {
#include "stg18/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg18/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg18/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg18/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[19] = {
#include "stg18/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg18/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[8] = {
#include "stg18/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg18/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg18/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg18/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[41] = {
#include "stg18/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg18/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[20] = {
#include "stg18/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg18/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg18/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg18/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[19] = {
#include "stg18/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg18/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[8] = {
#include "stg18/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg18/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg18/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg18/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[52] = {
#include "stg18/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg18/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[24] = {
#include "stg18/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg18/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg18/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg18/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[19] = {
#include "stg18/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg18/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[12] = {
#include "stg18/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg18/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg18/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg18/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[5] = {
#include "stg18/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg18/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[4] = {
#include "stg18/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg18/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg18/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg18/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[5] = {
#include "stg18/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg18/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[4] = {
#include "stg18/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg18/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg18/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg18/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[5] = {
#include "stg18/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg18/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[4] = {
#include "stg18/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg18/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg18/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg18/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[5] = {
#include "stg18/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg18/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[4] = {
#include "stg18/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg18/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg18/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg18/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[558] = {
#include "stg18/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg18/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[237] = {
#include "stg18/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg18/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg18/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg18/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[153] = {
#include "stg18/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg18/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[57] = {
#include "stg18/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg18/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg18/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg18/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[82] = {
#include "stg18/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg18/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[37] = {
#include "stg18/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg18/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg18/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg18/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[78] = {
#include "stg18/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg18/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[31] = {
#include "stg18/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg18/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg18/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg18/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[95] = {
#include "stg18/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg18/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[42] = {
#include "stg18/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg18/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg18/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg18/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[67] = {
#include "stg18/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg18/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[29] = {
#include "stg18/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg18/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg18/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg18/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[144] = {
#include "stg18/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg18/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[64] = {
#include "stg18/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg18/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg18/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg18/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[141] = {
#include "stg18/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg18/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[64] = {
#include "stg18/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg18/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg18/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg18/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[190] = {
#include "stg18/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg18/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[74] = {
#include "stg18/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg18/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg18/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg18/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[30] = {
#include "stg18/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg18/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[18] = {
#include "stg18/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg18/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg18/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg18/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[30] = {
#include "stg18/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg18/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[18] = {
#include "stg18/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg18/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg18/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg18/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[30] = {
#include "stg18/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg18/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[18] = {
#include "stg18/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg18/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg18/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg18/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[30] = {
#include "stg18/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg18/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[18] = {
#include "stg18/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg18/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg18/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg18/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[42] = {
#include "stg18/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg18/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[12] = {
#include "stg18/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg18/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg18/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg18/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[18] = {
#include "stg18/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg18/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[12] = {
#include "stg18/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg18/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg18/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg18/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[443] = {
#include "stg18/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg18/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[155] = {
#include "stg18/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg18/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg18/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg18/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[220] = {
#include "stg18/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg18/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[93] = {
#include "stg18/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg18/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg18/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg18/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[97] = {
#include "stg18/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg18/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[42] = {
#include "stg18/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg18/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg18/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg18/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[97] = {
#include "stg18/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg18/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[42] = {
#include "stg18/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg18/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg18/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg18/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[103] = {
#include "stg18/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg18/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[42] = {
#include "stg18/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg18/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg18/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg18/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[15] = {
#include "stg18/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg18/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[12] = {
#include "stg18/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg18/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg18/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg18/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[30] = {
#include "stg18/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg18/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[24] = {
#include "stg18/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg18/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg18/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg18/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[416] = {
#include "stg18/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg18/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[167] = {
#include "stg18/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg18/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg18/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg18/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[96] = {
#include "stg18/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg18/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[41] = {
#include "stg18/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg18/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg18/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg18/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[42] = {
#include "stg18/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg18/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[20] = {
#include "stg18/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg18/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg18/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg18/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[116] = {
#include "stg18/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg18/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[44] = {
#include "stg18/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg18/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg18/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg18/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[5] = {
#include "stg18/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg18/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[4] = {
#include "stg18/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg18/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg18/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg18/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[102] = {
#include "stg18/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg18/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[48] = {
#include "stg18/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg18/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg18/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg18/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[30] = {
#include "stg18/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg18/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[18] = {
#include "stg18/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg18/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg18/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg18/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[30] = {
#include "stg18/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg18/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[18] = {
#include "stg18/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg18/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg18/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg18/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[30] = {
#include "stg18/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg18/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[18] = {
#include "stg18/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg18/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg18/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg18/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[30] = {
#include "stg18/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg18/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[18] = {
#include "stg18/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg18/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg18/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg18/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[35] = {
#include "stg18/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg18/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[16] = {
#include "stg18/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg18/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg18/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg18/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[35] = {
#include "stg18/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg18/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[16] = {
#include "stg18/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg18/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg18/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg18/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[215] = {
#include "stg18/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg18/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[55] = {
#include "stg18/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg18/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg18/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg18/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[221] = {
#include "stg18/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg18/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[56] = {
#include "stg18/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg18/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg18/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg18/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[208] = {
#include "stg18/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg18/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[55] = {
#include "stg18/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg18/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg18/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg18/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[219] = {
#include "stg18/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg18/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[56] = {
#include "stg18/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg18/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg18/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg18/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[43] = {
#include "stg18/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg18/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[17] = {
#include "stg18/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg18/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg18/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg18/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[28] = {
#include "stg18/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg18/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[12] = {
#include "stg18/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg18/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg18/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg18/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[28] = {
#include "stg18/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg18/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[12] = {
#include "stg18/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg18/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg18/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg18/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[28] = {
#include "stg18/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg18/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[12] = {
#include "stg18/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg18/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg18/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg18/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[52] = {
#include "stg18/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg18/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[24] = {
#include "stg18/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg18/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg18/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg18/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[72] = {
#include "stg18/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg18/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[30] = {
#include "stg18/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg18/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg18/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg18/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[22] = {
#include "stg18/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg18/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[12] = {
#include "stg18/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg18/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg18/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg18/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[25] = {
#include "stg18/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg18/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[12] = {
#include "stg18/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg18/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg18/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg18/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[40] = {
#include "stg18/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg18/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[18] = {
#include "stg18/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg18/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg18/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg18/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[37] = {
#include "stg18/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg18/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[18] = {
#include "stg18/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg18/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg18/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg18/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[62] = {
#include "stg18/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg18/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[24] = {
#include "stg18/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg18/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg18/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg18/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[48] = {
#include "stg18/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg18/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[18] = {
#include "stg18/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg18/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg18/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg18/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[48] = {
#include "stg18/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg18/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[18] = {
#include "stg18/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg18/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg18/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg18/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[48] = {
#include "stg18/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg18/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[18] = {
#include "stg18/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg18/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg18/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg18/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[48] = {
#include "stg18/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg18/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[18] = {
#include "stg18/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg18/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg18/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg18/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[48] = {
#include "stg18/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg18/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[18] = {
#include "stg18/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg18/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg18/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg18/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[16] = {
#include "stg18/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg18/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[8] = {
#include "stg18/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg18/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg18/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg18/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[16] = {
#include "stg18/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg18/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[8] = {
#include "stg18/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg18/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg18/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg18/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[15] = {
#include "stg18/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg18/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[8] = {
#include "stg18/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg18/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg18/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg18/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[21] = {
#include "stg18/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg18/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[9] = {
#include "stg18/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg18/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg18/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg18/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[243] = {
#include "stg18/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg18/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[64] = {
#include "stg18/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg18/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg18/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg18/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[10] = {
#include "stg18/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg18/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[6] = {
#include "stg18/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg18/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg18/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg18/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[50] = {
#include "stg18/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg18/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[24] = {
#include "stg18/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg18/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg18/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg18/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[12] = {
#include "stg18/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg18/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[7] = {
#include "stg18/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg18/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg18/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg18/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[17] = {
#include "stg18/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg18/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[9] = {
#include "stg18/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg18/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg18/terrain/collision_76.inc"
};
static Sint16 draw_139_polygons[111] = {
#include "stg18/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[103] = {
#include "stg18/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg18/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg18/terrain/draw_139.inc"
};
static Sint16 draw_138_polygons[25] = {
#include "stg18/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[19] = {
#include "stg18/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg18/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg18/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[79] = {
#include "stg18/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[43] = {
#include "stg18/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg18/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg18/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[91] = {
#include "stg18/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[51] = {
#include "stg18/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg18/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg18/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[91] = {
#include "stg18/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[51] = {
#include "stg18/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg18/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg18/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[183] = {
#include "stg18/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[103] = {
#include "stg18/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg18/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg18/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[365] = {
#include "stg18/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[223] = {
#include "stg18/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg18/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg18/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[25] = {
#include "stg18/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[19] = {
#include "stg18/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg18/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg18/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[37] = {
#include "stg18/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[35] = {
#include "stg18/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg18/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg18/terrain/draw_131.inc"
};
static Sint16 draw_130_polygons[71] = {
#include "stg18/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[51] = {
#include "stg18/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg18/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg18/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[71] = {
#include "stg18/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[51] = {
#include "stg18/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg18/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg18/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[79] = {
#include "stg18/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[59] = {
#include "stg18/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg18/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg18/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[479] = {
#include "stg18/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[303] = {
#include "stg18/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg18/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg18/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[25] = {
#include "stg18/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[19] = {
#include "stg18/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg18/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg18/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[85] = {
#include "stg18/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[59] = {
#include "stg18/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg18/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg18/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[487] = {
#include "stg18/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[311] = {
#include "stg18/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg18/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg18/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[25] = {
#include "stg18/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[19] = {
#include "stg18/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg18/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg18/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[79] = {
#include "stg18/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[59] = {
#include "stg18/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg18/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg18/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[79] = {
#include "stg18/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[59] = {
#include "stg18/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg18/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg18/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[215] = {
#include "stg18/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[151] = {
#include "stg18/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg18/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg18/terrain/draw_120.inc"
};
static Sint16 draw_119_polygons[215] = {
#include "stg18/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[151] = {
#include "stg18/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg18/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg18/terrain/draw_119.inc"
};
static Sint16 draw_118_polygons[39] = {
#include "stg18/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[35] = {
#include "stg18/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg18/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg18/terrain/draw_118.inc"
};
static Sint16 draw_117_polygons[39] = {
#include "stg18/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[35] = {
#include "stg18/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg18/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg18/terrain/draw_117.inc"
};
static Sint16 draw_116_polygons[73] = {
#include "stg18/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[51] = {
#include "stg18/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg18/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg18/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[71] = {
#include "stg18/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[51] = {
#include "stg18/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg18/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg18/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[43] = {
#include "stg18/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[27] = {
#include "stg18/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg18/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg18/terrain/draw_114.inc"
};
static Sint16 draw_113_polygons[43] = {
#include "stg18/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[27] = {
#include "stg18/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg18/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg18/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[43] = {
#include "stg18/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[27] = {
#include "stg18/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg18/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg18/terrain/draw_112.inc"
};
static Sint16 draw_111_polygons[157] = {
#include "stg18/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[123] = {
#include "stg18/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg18/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg18/terrain/draw_111.inc"
};
static Sint16 draw_110_polygons[211] = {
#include "stg18/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[139] = {
#include "stg18/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg18/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg18/terrain/draw_110.inc"
};
static Sint16 draw_108_polygons[51] = {
#include "stg18/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[35] = {
#include "stg18/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg18/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg18/terrain/draw_109.inc"
};
static Sint16 draw_107_polygons[183] = {
#include "stg18/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[127] = {
#include "stg18/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg18/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg18/terrain/draw_107.inc"
};
static Sint16 draw_106_polygons[425] = {
#include "stg18/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[251] = {
#include "stg18/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg18/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg18/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[125] = {
#include "stg18/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[75] = {
#include "stg18/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg18/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg18/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[51] = {
#include "stg18/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[43] = {
#include "stg18/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg18/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg18/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[95] = {
#include "stg18/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[63] = {
#include "stg18/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg18/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg18/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[51] = {
#include "stg18/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[43] = {
#include "stg18/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg18/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg18/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[95] = {
#include "stg18/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[63] = {
#include "stg18/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg18/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg18/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[51] = {
#include "stg18/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[43] = {
#include "stg18/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg18/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg18/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[95] = {
#include "stg18/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[63] = {
#include "stg18/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg18/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg18/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[51] = {
#include "stg18/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[43] = {
#include "stg18/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg18/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg18/terrain/draw_98.inc"
};
static Sint16 draw_97_polygons[95] = {
#include "stg18/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[63] = {
#include "stg18/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg18/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg18/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[153] = {
#include "stg18/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[75] = {
#include "stg18/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg18/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg18/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[269] = {
#include "stg18/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[139] = {
#include "stg18/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg18/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg18/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[267] = {
#include "stg18/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[139] = {
#include "stg18/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg18/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg18/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[189] = {
#include "stg18/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[135] = {
#include "stg18/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg18/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg18/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[139] = {
#include "stg18/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[95] = {
#include "stg18/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg18/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg18/terrain/draw_92.inc"
};
static Sint16 draw_91_polygons[75] = {
#include "stg18/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[59] = {
#include "stg18/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg18/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg18/terrain/draw_91.inc"
};
static Sint16 draw_90_polygons[183] = {
#include "stg18/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[131] = {
#include "stg18/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg18/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg18/terrain/draw_90.inc"
};
static Sint16 draw_89_polygons[103] = {
#include "stg18/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[75] = {
#include "stg18/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg18/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg18/terrain/draw_89.inc"
};
static Sint16 draw_88_polygons[103] = {
#include "stg18/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[75] = {
#include "stg18/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg18/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg18/terrain/draw_88.inc"
};
static Sint16 draw_87_polygons[103] = {
#include "stg18/terrain/draw_87_polygons.inc"
};
static Sint32 draw_87_vertices[75] = {
#include "stg18/terrain/draw_87_vertices.inc"
};
static NJS_CNK_MODEL draw_87_model = {
#include "stg18/terrain/draw_87_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg18/terrain/draw_87.inc"
};
static Sint16 draw_86_polygons[103] = {
#include "stg18/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[75] = {
#include "stg18/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg18/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg18/terrain/draw_86.inc"
};
static Sint16 draw_85_polygons[1401] = {
#include "stg18/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[783] = {
#include "stg18/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg18/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg18/terrain/draw_85.inc"
};
static Sint16 draw_84_polygons[1401] = {
#include "stg18/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[735] = {
#include "stg18/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg18/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg18/terrain/draw_84.inc"
};
static Sint16 draw_83_polygons[1383] = {
#include "stg18/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[819] = {
#include "stg18/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg18/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg18/terrain/draw_83.inc"
};
static Sint16 draw_82_polygons[1383] = {
#include "stg18/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[819] = {
#include "stg18/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg18/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg18/terrain/draw_82.inc"
};
static Sint16 draw_81_polygons[141] = {
#include "stg18/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[91] = {
#include "stg18/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg18/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg18/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[43] = {
#include "stg18/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[35] = {
#include "stg18/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg18/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg18/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[445] = {
#include "stg18/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[227] = {
#include "stg18/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg18/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg18/terrain/draw_79.inc"
};
static Sint16 draw_78_polygons[445] = {
#include "stg18/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[227] = {
#include "stg18/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg18/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg18/terrain/draw_78.inc"
};
static Sint16 draw_77_polygons[183] = {
#include "stg18/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[123] = {
#include "stg18/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg18/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg18/terrain/draw_77.inc"
};
static Sint16 draw_76_polygons[51] = {
#include "stg18/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[35] = {
#include "stg18/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg18/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg18/terrain/draw_76.inc"
};
static Sint16 draw_75_polygons[201] = {
#include "stg18/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[171] = {
#include "stg18/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg18/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg18/terrain/draw_75.inc"
};
static Sint16 draw_73_polygons[175] = {
#include "stg18/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[99] = {
#include "stg18/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg18/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg18/terrain/draw_74.inc"
};
static Sint16 draw_71_polygons[217] = {
#include "stg18/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[155] = {
#include "stg18/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg18/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg18/terrain/draw_72.inc"
};
static Sint16 draw_70_polygons[287] = {
#include "stg18/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[151] = {
#include "stg18/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg18/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg18/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[171] = {
#include "stg18/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[107] = {
#include "stg18/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg18/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg18/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[325] = {
#include "stg18/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[203] = {
#include "stg18/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg18/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg18/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[297] = {
#include "stg18/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[167] = {
#include "stg18/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg18/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg18/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[225] = {
#include "stg18/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[171] = {
#include "stg18/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg18/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg18/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[91] = {
#include "stg18/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[51] = {
#include "stg18/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg18/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg18/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[89] = {
#include "stg18/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[51] = {
#include "stg18/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg18/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg18/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[287] = {
#include "stg18/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[195] = {
#include "stg18/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg18/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg18/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[283] = {
#include "stg18/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[187] = {
#include "stg18/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg18/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg18/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[143] = {
#include "stg18/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[95] = {
#include "stg18/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg18/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg18/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[169] = {
#include "stg18/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[111] = {
#include "stg18/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg18/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg18/terrain/draw_60.inc"
};
static Sint16 draw_55_polygons[37] = {
#include "stg18/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[27] = {
#include "stg18/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg18/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg18/terrain/draw_59.inc"
};
static Sint16 draw_54_polygons[329] = {
#include "stg18/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[175] = {
#include "stg18/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg18/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg18/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[43] = {
#include "stg18/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[39] = {
#include "stg18/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg18/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg18/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[49] = {
#include "stg18/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[51] = {
#include "stg18/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg18/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg18/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[45] = {
#include "stg18/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[35] = {
#include "stg18/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg18/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg18/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[115] = {
#include "stg18/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[59] = {
#include "stg18/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg18/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg18/terrain/draw_50.inc"
};
static Sint16 draw_49_polygons[115] = {
#include "stg18/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[67] = {
#include "stg18/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg18/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg18/terrain/draw_49.inc"
};
static Sint16 draw_48_polygons[135] = {
#include "stg18/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[95] = {
#include "stg18/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg18/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg18/terrain/draw_48.inc"
};
static Sint16 draw_47_polygons[239] = {
#include "stg18/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[139] = {
#include "stg18/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg18/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg18/terrain/draw_47.inc"
};
static Sint16 draw_45_polygons[267] = {
#include "stg18/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[167] = {
#include "stg18/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg18/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg18/terrain/draw_46.inc"
};
static Sint16 draw_44_polygons[219] = {
#include "stg18/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[151] = {
#include "stg18/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg18/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg18/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[225] = {
#include "stg18/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[143] = {
#include "stg18/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg18/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg18/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[479] = {
#include "stg18/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[243] = {
#include "stg18/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg18/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg18/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[479] = {
#include "stg18/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[243] = {
#include "stg18/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg18/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg18/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[471] = {
#include "stg18/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[247] = {
#include "stg18/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg18/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg18/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[49] = {
#include "stg18/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[51] = {
#include "stg18/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg18/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg18/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[95] = {
#include "stg18/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[71] = {
#include "stg18/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg18/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg18/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[29] = {
#include "stg18/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[23] = {
#include "stg18/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg18/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg18/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[55] = {
#include "stg18/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[59] = {
#include "stg18/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg18/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg18/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[321] = {
#include "stg18/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[239] = {
#include "stg18/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg18/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg18/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[157] = {
#include "stg18/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[151] = {
#include "stg18/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg18/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg18/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[365] = {
#include "stg18/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[195] = {
#include "stg18/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg18/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg18/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[1375] = {
#include "stg18/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[811] = {
#include "stg18/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg18/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg18/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[537] = {
#include "stg18/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[359] = {
#include "stg18/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg18/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg18/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[215] = {
#include "stg18/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[175] = {
#include "stg18/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg18/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg18/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[193] = {
#include "stg18/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[127] = {
#include "stg18/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg18/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg18/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[215] = {
#include "stg18/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[155] = {
#include "stg18/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg18/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg18/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[215] = {
#include "stg18/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[175] = {
#include "stg18/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg18/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg18/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[193] = {
#include "stg18/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[127] = {
#include "stg18/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg18/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg18/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[215] = {
#include "stg18/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[155] = {
#include "stg18/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg18/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg18/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[141] = {
#include "stg18/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[79] = {
#include "stg18/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg18/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg18/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[63] = {
#include "stg18/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[51] = {
#include "stg18/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg18/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg18/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[215] = {
#include "stg18/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[155] = {
#include "stg18/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg18/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg18/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[193] = {
#include "stg18/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[131] = {
#include "stg18/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg18/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg18/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[471] = {
#include "stg18/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[311] = {
#include "stg18/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg18/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg18/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[75] = {
#include "stg18/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[67] = {
#include "stg18/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg18/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg18/terrain/draw_19.inc"
};
static Sint16 draw_17_polygons[107] = {
#include "stg18/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[83] = {
#include "stg18/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg18/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg18/terrain/draw_18.inc"
};
static Sint16 draw_16_polygons[51] = {
#include "stg18/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[43] = {
#include "stg18/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg18/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg18/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[77] = {
#include "stg18/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[75] = {
#include "stg18/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg18/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg18/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[37] = {
#include "stg18/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[35] = {
#include "stg18/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg18/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg18/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[113] = {
#include "stg18/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[91] = {
#include "stg18/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg18/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg18/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[31] = {
#include "stg18/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[27] = {
#include "stg18/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg18/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg18/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[131] = {
#include "stg18/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[83] = {
#include "stg18/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg18/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg18/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[187] = {
#include "stg18/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[131] = {
#include "stg18/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg18/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg18/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[189] = {
#include "stg18/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[135] = {
#include "stg18/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg18/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg18/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[141] = {
#include "stg18/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[99] = {
#include "stg18/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg18/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg18/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[43] = {
#include "stg18/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[35] = {
#include "stg18/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg18/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg18/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[417] = {
#include "stg18/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[227] = {
#include "stg18/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg18/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg18/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[221] = {
#include "stg18/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[131] = {
#include "stg18/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg18/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg18/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[51] = {
#include "stg18/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[43] = {
#include "stg18/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg18/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg18/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[69] = {
#include "stg18/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[63] = {
#include "stg18/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg18/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg18/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[63] = {
#include "stg18/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[59] = {
#include "stg18/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg18/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg18/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[63] = {
#include "stg18/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[59] = {
#include "stg18/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg18/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg18/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[63] = {
#include "stg18/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[59] = {
#include "stg18/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg18/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg18/terrain/draw_0.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg18/terrain/collision_55.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg18/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg18/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg18/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg18/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg18/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg18/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg18/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg18/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg18/terrain/draw_17.inc"
};
