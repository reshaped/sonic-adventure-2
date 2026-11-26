#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg04/white_jungle.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char white_jungle_land_filenames[347] = {
#include "stg04/terrain/white_jungle_land_filenames.inc"
};

/* STG04.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between white_jungle.c and objects.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[338];
struct land_table white_jungle_land;
static NJS_TEXNAME textures[26];
NJS_TEXLIST white_jungle_terrain_textures;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[30];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[21];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[34];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[24];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[61];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[29];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[50];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[30];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[63];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[36];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[95];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[48];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[34];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[18];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[28];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[15];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[21];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[12];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[118];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[61];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[64];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[33];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[85];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[48];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[42];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[20];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[73];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[40];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[139];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[67];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[31];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[18];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[48];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[23];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[34];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[18];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[55];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[30];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[31];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[18];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[43];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[22];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[34];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[18];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[37];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[22];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[34];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[18];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[91];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[45];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[22];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[15];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[35];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[19];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[61];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[38];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[46];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[26];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[43];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[26];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[37];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[22];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[83];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[43];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[32];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[19];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[123];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[58];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[43];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[22];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[186];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[95];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[60];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[32];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[28];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[15];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[69];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[30];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[37];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[18];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[47];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[23];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[51];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[26];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[43];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[28];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[80];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[44];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[118];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[55];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[79];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[37];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[567];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[310];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[78];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[36];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[77];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[30];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[149];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[76];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[135];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[53];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[50];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[26];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[244];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[106];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[153];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[74];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[108];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[51];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[92];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[39];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[255];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[106];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[30];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[18];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[31];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[16];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[42];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[30];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[91];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[48];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[79];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[47];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[160];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[71];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[297];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[160];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[297];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[160];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[297];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[160];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[297];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[160];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[617];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[338];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[114];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[76];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[27];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[17];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[30];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[18];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[168];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[71];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[186];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[72];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[28];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[18];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[9];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[8];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[28];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[15];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[21];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[20];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[24];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[20];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[111];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[60];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[105];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[60];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[117];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[60];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[399];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[210];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[115];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[62];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[64];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[26];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[323];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[169];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[387];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[210];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[84];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[50];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[63];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[40];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[102];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[50];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[69];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[30];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[69];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[30];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static Sint16 draw_205_polygons[117];
static Sint32 draw_205_vertices[91];
static NJS_CNK_MODEL draw_205_model;
static NJS_CNK_OBJECT draw_205;
static Sint16 draw_80_polygons[121];
static Sint32 draw_80_vertices[91];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_204;
static Sint16 draw_203_polygons[69];
static Sint32 draw_203_vertices[51];
static NJS_CNK_MODEL draw_203_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_201_polygons[69];
static Sint32 draw_201_vertices[51];
static NJS_CNK_MODEL draw_201_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_200_polygons[221];
static Sint32 draw_200_vertices[207];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_200;
static Sint16 draw_42_polygons[379];
static Sint32 draw_42_vertices[271];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_76_polygons[515];
static Sint32 draw_76_vertices[439];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_5_polygons[263];
static Sint32 draw_5_vertices[203];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_3_polygons[333];
static Sint32 draw_3_vertices[223];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_0_polygons[337];
static Sint32 draw_0_vertices[259];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_65_polygons[1115];
static Sint32 draw_65_vertices[895];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_82_polygons[1185];
static Sint32 draw_82_vertices[923];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_190;
static Sint16 draw_74_polygons[209];
static Sint32 draw_74_vertices[179];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_188;
static Sint16 draw_13_polygons[251];
static Sint32 draw_13_vertices[163];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_186_polygons[253];
static Sint32 draw_186_vertices[187];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_186;
static Sint16 draw_7_polygons[353];
static Sint32 draw_7_vertices[243];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_180_polygons[237];
static Sint32 draw_180_vertices[215];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_179_polygons[223];
static Sint32 draw_179_vertices[207];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_177_polygons[451];
static Sint32 draw_177_vertices[351];
static NJS_CNK_MODEL draw_177_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_176_polygons[65];
static Sint32 draw_176_vertices[51];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_167_polygons[223];
static Sint32 draw_167_vertices[207];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_166_polygons[71];
static Sint32 draw_166_vertices[51];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_165_polygons[237];
static Sint32 draw_165_vertices[215];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_31_polygons[463];
static Sint32 draw_31_vertices[343];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_54_polygons[165];
static Sint32 draw_54_vertices[115];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_46_polygons[1115];
static Sint32 draw_46_vertices[1011];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_153_polygons[1059];
static Sint32 draw_153_vertices[899];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_149_polygons[71];
static Sint32 draw_149_vertices[51];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_28_polygons[977];
static Sint32 draw_28_vertices[739];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_6_polygons[343];
static Sint32 draw_6_vertices[255];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_134_polygons[1155];
static Sint32 draw_134_vertices[867];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[595];
static Sint32 draw_133_vertices[407];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_130_polygons[195];
static Sint32 draw_130_vertices[151];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[467];
static Sint32 draw_129_vertices[299];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[595];
static Sint32 draw_128_vertices[395];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[595];
static Sint32 draw_127_vertices[515];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[189];
static Sint32 draw_126_vertices[171];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_124_polygons[235];
static Sint32 draw_124_vertices[215];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[235];
static Sint32 draw_123_vertices[215];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[221];
static Sint32 draw_122_vertices[207];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[69];
static Sint32 draw_121_vertices[51];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[603];
static Sint32 draw_120_vertices[491];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_60_polygons[349];
static Sint32 draw_60_vertices[271];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_37_polygons[359];
static Sint32 draw_37_vertices[247];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[1847];
static Sint32 draw_115_vertices[1723];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[383];
static Sint32 draw_114_vertices[367];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_107_polygons[43];
static Sint32 draw_107_vertices[39];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_106_polygons[307];
static Sint32 draw_106_vertices[215];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[35];
static Sint32 draw_105_vertices[35];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_102_polygons[171];
static Sint32 draw_102_vertices[111];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[261];
static Sint32 draw_101_vertices[139];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[91];
static Sint32 draw_100_vertices[63];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[97];
static Sint32 draw_99_vertices[87];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[957];
static Sint32 draw_98_vertices[587];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_97_polygons[389];
static Sint32 draw_97_vertices[303];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[173];
static Sint32 draw_96_vertices[139];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[29];
static Sint32 draw_95_vertices[27];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[93];
static Sint32 draw_94_vertices[67];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[239];
static Sint32 draw_93_vertices[167];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[631];
static Sint32 draw_92_vertices[443];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_91_polygons[91];
static Sint32 draw_91_vertices[55];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_90_polygons[117];
static Sint32 draw_90_vertices[87];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_89_polygons[63];
static Sint32 draw_89_vertices[55];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_73_polygons[69];
static Sint32 draw_73_vertices[51];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[69];
static Sint32 draw_72_vertices[51];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[235];
static Sint32 draw_71_vertices[215];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[221];
static Sint32 draw_70_vertices[207];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[221];
static Sint32 draw_69_vertices[207];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[297];
static Sint32 draw_68_vertices[195];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[241];
static Sint32 draw_67_vertices[151];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_63_polygons[63];
static Sint32 draw_63_vertices[55];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_59_polygons[139];
static Sint32 draw_59_vertices[171];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_57_polygons[2119];
static Sint32 draw_57_vertices[1967];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[377];
static Sint32 draw_56_vertices[303];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[171];
static Sint32 draw_55_vertices[115];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_49_polygons[485];
static Sint32 draw_49_vertices[323];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_48_polygons[49];
static Sint32 draw_48_vertices[47];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_45_polygons[99];
static Sint32 draw_45_vertices[83];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_41_polygons[221];
static Sint32 draw_41_vertices[207];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[349];
static Sint32 draw_40_vertices[271];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_38_polygons[69];
static Sint32 draw_38_vertices[51];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_34_polygons[271];
static Sint32 draw_34_vertices[199];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[111];
static Sint32 draw_33_vertices[83];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[221];
static Sint32 draw_32_vertices[207];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_30_polygons[235];
static Sint32 draw_30_vertices[215];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[235];
static Sint32 draw_29_vertices[215];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_21_polygons[1073];
static Sint32 draw_21_vertices[775];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_1_polygons[799];
static Sint32 draw_1_vertices[615];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_120;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_146;
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
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
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
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
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
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[338] = {
#include "stg04/terrain/entries.inc"
};
struct land_table white_jungle_land = {
#include "stg04/terrain/white_jungle_land.inc"
};
static NJS_TEXNAME textures[26] = {
#include "stg04/terrain/textures.inc"
};
NJS_TEXLIST white_jungle_terrain_textures = {
#include "stg04/terrain/white_jungle_terrain_textures.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg04/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[30] = {
#include "stg04/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg04/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[21] = {
#include "stg04/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg04/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg04/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg04/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[34] = {
#include "stg04/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg04/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[24] = {
#include "stg04/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg04/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg04/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg04/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[61] = {
#include "stg04/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg04/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[29] = {
#include "stg04/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg04/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg04/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg04/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[50] = {
#include "stg04/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg04/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[30] = {
#include "stg04/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg04/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg04/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg04/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[63] = {
#include "stg04/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg04/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[36] = {
#include "stg04/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg04/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg04/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg04/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[95] = {
#include "stg04/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg04/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[48] = {
#include "stg04/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg04/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg04/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg04/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[34] = {
#include "stg04/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg04/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[18] = {
#include "stg04/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg04/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg04/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg04/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[28] = {
#include "stg04/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg04/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[15] = {
#include "stg04/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg04/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg04/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg04/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[21] = {
#include "stg04/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg04/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[12] = {
#include "stg04/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg04/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg04/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg04/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[118] = {
#include "stg04/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg04/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[61] = {
#include "stg04/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg04/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg04/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg04/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[64] = {
#include "stg04/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg04/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[33] = {
#include "stg04/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg04/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg04/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg04/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[85] = {
#include "stg04/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg04/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[48] = {
#include "stg04/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg04/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg04/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg04/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[42] = {
#include "stg04/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg04/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[20] = {
#include "stg04/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg04/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg04/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg04/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[73] = {
#include "stg04/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg04/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[40] = {
#include "stg04/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg04/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg04/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg04/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[139] = {
#include "stg04/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg04/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[67] = {
#include "stg04/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg04/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg04/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg04/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[31] = {
#include "stg04/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg04/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[18] = {
#include "stg04/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg04/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg04/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg04/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[48] = {
#include "stg04/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg04/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[23] = {
#include "stg04/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg04/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg04/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg04/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[34] = {
#include "stg04/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg04/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[18] = {
#include "stg04/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg04/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg04/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg04/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[55] = {
#include "stg04/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg04/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[30] = {
#include "stg04/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg04/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg04/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg04/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[31] = {
#include "stg04/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg04/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[18] = {
#include "stg04/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg04/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg04/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg04/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[43] = {
#include "stg04/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg04/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[22] = {
#include "stg04/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg04/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg04/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg04/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[34] = {
#include "stg04/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg04/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[18] = {
#include "stg04/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg04/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg04/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg04/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[37] = {
#include "stg04/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg04/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[22] = {
#include "stg04/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg04/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg04/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg04/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[34] = {
#include "stg04/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg04/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[18] = {
#include "stg04/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg04/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg04/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg04/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[91] = {
#include "stg04/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg04/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[45] = {
#include "stg04/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg04/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg04/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg04/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[22] = {
#include "stg04/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg04/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[15] = {
#include "stg04/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg04/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg04/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg04/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[35] = {
#include "stg04/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg04/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[19] = {
#include "stg04/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg04/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg04/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg04/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[61] = {
#include "stg04/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg04/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[38] = {
#include "stg04/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg04/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg04/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg04/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[46] = {
#include "stg04/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg04/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[26] = {
#include "stg04/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg04/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg04/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg04/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[43] = {
#include "stg04/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg04/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[26] = {
#include "stg04/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg04/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg04/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg04/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[37] = {
#include "stg04/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg04/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[22] = {
#include "stg04/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg04/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg04/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg04/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[83] = {
#include "stg04/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg04/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[43] = {
#include "stg04/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg04/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg04/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg04/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[32] = {
#include "stg04/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg04/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[19] = {
#include "stg04/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg04/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg04/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg04/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[123] = {
#include "stg04/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg04/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[58] = {
#include "stg04/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg04/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg04/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg04/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[43] = {
#include "stg04/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg04/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[22] = {
#include "stg04/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg04/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg04/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg04/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[186] = {
#include "stg04/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg04/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[95] = {
#include "stg04/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg04/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg04/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg04/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[60] = {
#include "stg04/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg04/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[32] = {
#include "stg04/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg04/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg04/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg04/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[28] = {
#include "stg04/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg04/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[15] = {
#include "stg04/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg04/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg04/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg04/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[69] = {
#include "stg04/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg04/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[30] = {
#include "stg04/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg04/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg04/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg04/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[37] = {
#include "stg04/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg04/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[18] = {
#include "stg04/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg04/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg04/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg04/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[47] = {
#include "stg04/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg04/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[23] = {
#include "stg04/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg04/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg04/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg04/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[51] = {
#include "stg04/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg04/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[26] = {
#include "stg04/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg04/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg04/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg04/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[43] = {
#include "stg04/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg04/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[28] = {
#include "stg04/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg04/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg04/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg04/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[80] = {
#include "stg04/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg04/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[44] = {
#include "stg04/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg04/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg04/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg04/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[118] = {
#include "stg04/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg04/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[55] = {
#include "stg04/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg04/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg04/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg04/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[79] = {
#include "stg04/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg04/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[37] = {
#include "stg04/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg04/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg04/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg04/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[567] = {
#include "stg04/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg04/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[310] = {
#include "stg04/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg04/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg04/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg04/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[78] = {
#include "stg04/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg04/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[36] = {
#include "stg04/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg04/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg04/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg04/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[77] = {
#include "stg04/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg04/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[30] = {
#include "stg04/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg04/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg04/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg04/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[149] = {
#include "stg04/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg04/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[76] = {
#include "stg04/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg04/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg04/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg04/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[135] = {
#include "stg04/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg04/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[53] = {
#include "stg04/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg04/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg04/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg04/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[50] = {
#include "stg04/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg04/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[26] = {
#include "stg04/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg04/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg04/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg04/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[244] = {
#include "stg04/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg04/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[106] = {
#include "stg04/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg04/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg04/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg04/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[153] = {
#include "stg04/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg04/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[74] = {
#include "stg04/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg04/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg04/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg04/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[108] = {
#include "stg04/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg04/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[51] = {
#include "stg04/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg04/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg04/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg04/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[92] = {
#include "stg04/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg04/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[39] = {
#include "stg04/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg04/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg04/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg04/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[255] = {
#include "stg04/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg04/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[106] = {
#include "stg04/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg04/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg04/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg04/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[30] = {
#include "stg04/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg04/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[18] = {
#include "stg04/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg04/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg04/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg04/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[31] = {
#include "stg04/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg04/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[16] = {
#include "stg04/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg04/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg04/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg04/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[42] = {
#include "stg04/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg04/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[30] = {
#include "stg04/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg04/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg04/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg04/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[91] = {
#include "stg04/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg04/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[48] = {
#include "stg04/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg04/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg04/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg04/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[79] = {
#include "stg04/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg04/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[47] = {
#include "stg04/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg04/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg04/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg04/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[160] = {
#include "stg04/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg04/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[71] = {
#include "stg04/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg04/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg04/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg04/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[297] = {
#include "stg04/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg04/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[160] = {
#include "stg04/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg04/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg04/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg04/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[297] = {
#include "stg04/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg04/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[160] = {
#include "stg04/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg04/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg04/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg04/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[297] = {
#include "stg04/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg04/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[160] = {
#include "stg04/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg04/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg04/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg04/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[297] = {
#include "stg04/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg04/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[160] = {
#include "stg04/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg04/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg04/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg04/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[617] = {
#include "stg04/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg04/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[338] = {
#include "stg04/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg04/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg04/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg04/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[114] = {
#include "stg04/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg04/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[76] = {
#include "stg04/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg04/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg04/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg04/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[27] = {
#include "stg04/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg04/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[17] = {
#include "stg04/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg04/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg04/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg04/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[30] = {
#include "stg04/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg04/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[18] = {
#include "stg04/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg04/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg04/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg04/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[168] = {
#include "stg04/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg04/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[71] = {
#include "stg04/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg04/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg04/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg04/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[186] = {
#include "stg04/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg04/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[72] = {
#include "stg04/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg04/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg04/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg04/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[28] = {
#include "stg04/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg04/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[18] = {
#include "stg04/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg04/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg04/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg04/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[9] = {
#include "stg04/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg04/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[8] = {
#include "stg04/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg04/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg04/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg04/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[28] = {
#include "stg04/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg04/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[15] = {
#include "stg04/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg04/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg04/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg04/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[21] = {
#include "stg04/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg04/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[20] = {
#include "stg04/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg04/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg04/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg04/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[24] = {
#include "stg04/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg04/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[20] = {
#include "stg04/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg04/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg04/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg04/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[111] = {
#include "stg04/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg04/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[60] = {
#include "stg04/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg04/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg04/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg04/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[105] = {
#include "stg04/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg04/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[60] = {
#include "stg04/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg04/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg04/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg04/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[117] = {
#include "stg04/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg04/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[60] = {
#include "stg04/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg04/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg04/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg04/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[399] = {
#include "stg04/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg04/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[210] = {
#include "stg04/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg04/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg04/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg04/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[115] = {
#include "stg04/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg04/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[62] = {
#include "stg04/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg04/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg04/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg04/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[64] = {
#include "stg04/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg04/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[26] = {
#include "stg04/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg04/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg04/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg04/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[323] = {
#include "stg04/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg04/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[169] = {
#include "stg04/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg04/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg04/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg04/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[387] = {
#include "stg04/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg04/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[210] = {
#include "stg04/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg04/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg04/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg04/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[84] = {
#include "stg04/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg04/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[50] = {
#include "stg04/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg04/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg04/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg04/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[63] = {
#include "stg04/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg04/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[40] = {
#include "stg04/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg04/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg04/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg04/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[102] = {
#include "stg04/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg04/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[50] = {
#include "stg04/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg04/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg04/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg04/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[69] = {
#include "stg04/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg04/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[30] = {
#include "stg04/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg04/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg04/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg04/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[69] = {
#include "stg04/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg04/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[30] = {
#include "stg04/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg04/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg04/terrain/collision_131.inc"
};
static Sint16 draw_205_polygons[117] = {
#include "stg04/terrain/draw_205_polygons.inc"
};
static Sint32 draw_205_vertices[91] = {
#include "stg04/terrain/draw_205_vertices.inc"
};
static NJS_CNK_MODEL draw_205_model = {
#include "stg04/terrain/draw_205_model.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg04/terrain/draw_205.inc"
};
static Sint16 draw_80_polygons[121] = {
#include "stg04/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[91] = {
#include "stg04/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg04/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg04/terrain/draw_204.inc"
};
static Sint16 draw_203_polygons[69] = {
#include "stg04/terrain/draw_203_polygons.inc"
};
static Sint32 draw_203_vertices[51] = {
#include "stg04/terrain/draw_203_vertices.inc"
};
static NJS_CNK_MODEL draw_203_model = {
#include "stg04/terrain/draw_203_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg04/terrain/draw_203.inc"
};
static Sint16 draw_201_polygons[69] = {
#include "stg04/terrain/draw_201_polygons.inc"
};
static Sint32 draw_201_vertices[51] = {
#include "stg04/terrain/draw_201_vertices.inc"
};
static NJS_CNK_MODEL draw_201_model = {
#include "stg04/terrain/draw_201_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg04/terrain/draw_201.inc"
};
static Sint16 draw_200_polygons[221] = {
#include "stg04/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[207] = {
#include "stg04/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg04/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg04/terrain/draw_200.inc"
};
static Sint16 draw_42_polygons[379] = {
#include "stg04/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[271] = {
#include "stg04/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg04/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg04/terrain/draw_199.inc"
};
static Sint16 draw_76_polygons[515] = {
#include "stg04/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[439] = {
#include "stg04/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg04/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg04/terrain/draw_197.inc"
};
static Sint16 draw_5_polygons[263] = {
#include "stg04/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[203] = {
#include "stg04/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg04/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg04/terrain/draw_195.inc"
};
static Sint16 draw_3_polygons[333] = {
#include "stg04/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[223] = {
#include "stg04/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg04/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg04/terrain/draw_194.inc"
};
static Sint16 draw_0_polygons[337] = {
#include "stg04/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[259] = {
#include "stg04/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg04/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg04/terrain/draw_192.inc"
};
static Sint16 draw_65_polygons[1115] = {
#include "stg04/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[895] = {
#include "stg04/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg04/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg04/terrain/draw_191.inc"
};
static Sint16 draw_82_polygons[1185] = {
#include "stg04/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[923] = {
#include "stg04/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg04/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg04/terrain/draw_190.inc"
};
static Sint16 draw_74_polygons[209] = {
#include "stg04/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[179] = {
#include "stg04/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg04/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg04/terrain/draw_188.inc"
};
static Sint16 draw_13_polygons[251] = {
#include "stg04/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[163] = {
#include "stg04/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg04/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg04/terrain/draw_187.inc"
};
static Sint16 draw_186_polygons[253] = {
#include "stg04/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[187] = {
#include "stg04/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg04/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg04/terrain/draw_186.inc"
};
static Sint16 draw_7_polygons[353] = {
#include "stg04/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[243] = {
#include "stg04/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg04/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg04/terrain/draw_185.inc"
};
static Sint16 draw_180_polygons[237] = {
#include "stg04/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[215] = {
#include "stg04/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg04/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg04/terrain/draw_180.inc"
};
static Sint16 draw_179_polygons[223] = {
#include "stg04/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[207] = {
#include "stg04/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg04/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg04/terrain/draw_179.inc"
};
static Sint16 draw_177_polygons[451] = {
#include "stg04/terrain/draw_177_polygons.inc"
};
static Sint32 draw_177_vertices[351] = {
#include "stg04/terrain/draw_177_vertices.inc"
};
static NJS_CNK_MODEL draw_177_model = {
#include "stg04/terrain/draw_177_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg04/terrain/draw_177.inc"
};
static Sint16 draw_176_polygons[65] = {
#include "stg04/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[51] = {
#include "stg04/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg04/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg04/terrain/draw_176.inc"
};
static Sint16 draw_167_polygons[223] = {
#include "stg04/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[207] = {
#include "stg04/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg04/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg04/terrain/draw_167.inc"
};
static Sint16 draw_166_polygons[71] = {
#include "stg04/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[51] = {
#include "stg04/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg04/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg04/terrain/draw_166.inc"
};
static Sint16 draw_165_polygons[237] = {
#include "stg04/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[215] = {
#include "stg04/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg04/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg04/terrain/draw_165.inc"
};
static Sint16 draw_31_polygons[463] = {
#include "stg04/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[343] = {
#include "stg04/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg04/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg04/terrain/draw_161.inc"
};
static Sint16 draw_54_polygons[165] = {
#include "stg04/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[115] = {
#include "stg04/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg04/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg04/terrain/draw_159.inc"
};
static Sint16 draw_46_polygons[1115] = {
#include "stg04/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[1011] = {
#include "stg04/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg04/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg04/terrain/draw_155.inc"
};
static Sint16 draw_153_polygons[1059] = {
#include "stg04/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[899] = {
#include "stg04/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg04/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg04/terrain/draw_153.inc"
};
static Sint16 draw_149_polygons[71] = {
#include "stg04/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[51] = {
#include "stg04/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg04/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg04/terrain/draw_149.inc"
};
static Sint16 draw_28_polygons[977] = {
#include "stg04/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[739] = {
#include "stg04/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg04/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg04/terrain/draw_147.inc"
};
static Sint16 draw_6_polygons[343] = {
#include "stg04/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[255] = {
#include "stg04/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg04/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg04/terrain/draw_145.inc"
};
static Sint16 draw_134_polygons[1155] = {
#include "stg04/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[867] = {
#include "stg04/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg04/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg04/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[595] = {
#include "stg04/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[407] = {
#include "stg04/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg04/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg04/terrain/draw_133.inc"
};
static Sint16 draw_130_polygons[195] = {
#include "stg04/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[151] = {
#include "stg04/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg04/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg04/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[467] = {
#include "stg04/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[299] = {
#include "stg04/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg04/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg04/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[595] = {
#include "stg04/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[395] = {
#include "stg04/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg04/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg04/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[595] = {
#include "stg04/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[515] = {
#include "stg04/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg04/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg04/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[189] = {
#include "stg04/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[171] = {
#include "stg04/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg04/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg04/terrain/draw_126.inc"
};
static Sint16 draw_124_polygons[235] = {
#include "stg04/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[215] = {
#include "stg04/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg04/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg04/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[235] = {
#include "stg04/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[215] = {
#include "stg04/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg04/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg04/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[221] = {
#include "stg04/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[207] = {
#include "stg04/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg04/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg04/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[69] = {
#include "stg04/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[51] = {
#include "stg04/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg04/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg04/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[603] = {
#include "stg04/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[491] = {
#include "stg04/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg04/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg04/terrain/draw_120.inc"
};
static Sint16 draw_60_polygons[349] = {
#include "stg04/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[271] = {
#include "stg04/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg04/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg04/terrain/draw_119.inc"
};
static Sint16 draw_37_polygons[359] = {
#include "stg04/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[247] = {
#include "stg04/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg04/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg04/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[1847] = {
#include "stg04/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[1723] = {
#include "stg04/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg04/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg04/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[383] = {
#include "stg04/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[367] = {
#include "stg04/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg04/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg04/terrain/draw_114.inc"
};
static Sint16 draw_107_polygons[43] = {
#include "stg04/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[39] = {
#include "stg04/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg04/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg04/terrain/draw_107.inc"
};
static Sint16 draw_106_polygons[307] = {
#include "stg04/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[215] = {
#include "stg04/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg04/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg04/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[35] = {
#include "stg04/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[35] = {
#include "stg04/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg04/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg04/terrain/draw_105.inc"
};
static Sint16 draw_102_polygons[171] = {
#include "stg04/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[111] = {
#include "stg04/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg04/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg04/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[261] = {
#include "stg04/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[139] = {
#include "stg04/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg04/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg04/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[91] = {
#include "stg04/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[63] = {
#include "stg04/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg04/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg04/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[97] = {
#include "stg04/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[87] = {
#include "stg04/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg04/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg04/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[957] = {
#include "stg04/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[587] = {
#include "stg04/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg04/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg04/terrain/draw_98.inc"
};
static Sint16 draw_97_polygons[389] = {
#include "stg04/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[303] = {
#include "stg04/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg04/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg04/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[173] = {
#include "stg04/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[139] = {
#include "stg04/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg04/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg04/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[29] = {
#include "stg04/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[27] = {
#include "stg04/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg04/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg04/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[93] = {
#include "stg04/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[67] = {
#include "stg04/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg04/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg04/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[239] = {
#include "stg04/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[167] = {
#include "stg04/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg04/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg04/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[631] = {
#include "stg04/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[443] = {
#include "stg04/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg04/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg04/terrain/draw_92.inc"
};
static Sint16 draw_91_polygons[91] = {
#include "stg04/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[55] = {
#include "stg04/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg04/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg04/terrain/draw_91.inc"
};
static Sint16 draw_90_polygons[117] = {
#include "stg04/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[87] = {
#include "stg04/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg04/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg04/terrain/draw_90.inc"
};
static Sint16 draw_89_polygons[63] = {
#include "stg04/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[55] = {
#include "stg04/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg04/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg04/terrain/draw_89.inc"
};
static Sint16 draw_73_polygons[69] = {
#include "stg04/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[51] = {
#include "stg04/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg04/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg04/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[69] = {
#include "stg04/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[51] = {
#include "stg04/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg04/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg04/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[235] = {
#include "stg04/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[215] = {
#include "stg04/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg04/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg04/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[221] = {
#include "stg04/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[207] = {
#include "stg04/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg04/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg04/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[221] = {
#include "stg04/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[207] = {
#include "stg04/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg04/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg04/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[297] = {
#include "stg04/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[195] = {
#include "stg04/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg04/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg04/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[241] = {
#include "stg04/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[151] = {
#include "stg04/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg04/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg04/terrain/draw_67.inc"
};
static Sint16 draw_63_polygons[63] = {
#include "stg04/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[55] = {
#include "stg04/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg04/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg04/terrain/draw_63.inc"
};
static Sint16 draw_59_polygons[139] = {
#include "stg04/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[171] = {
#include "stg04/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg04/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg04/terrain/draw_59.inc"
};
static Sint16 draw_57_polygons[2119] = {
#include "stg04/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[1967] = {
#include "stg04/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg04/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg04/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[377] = {
#include "stg04/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[303] = {
#include "stg04/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg04/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg04/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[171] = {
#include "stg04/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[115] = {
#include "stg04/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg04/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg04/terrain/draw_55.inc"
};
static Sint16 draw_49_polygons[485] = {
#include "stg04/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[323] = {
#include "stg04/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg04/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg04/terrain/draw_50.inc"
};
static Sint16 draw_48_polygons[49] = {
#include "stg04/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[47] = {
#include "stg04/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg04/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg04/terrain/draw_48.inc"
};
static Sint16 draw_45_polygons[99] = {
#include "stg04/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[83] = {
#include "stg04/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg04/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg04/terrain/draw_45.inc"
};
static Sint16 draw_41_polygons[221] = {
#include "stg04/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[207] = {
#include "stg04/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg04/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg04/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[349] = {
#include "stg04/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[271] = {
#include "stg04/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg04/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg04/terrain/draw_40.inc"
};
static Sint16 draw_38_polygons[69] = {
#include "stg04/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[51] = {
#include "stg04/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg04/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg04/terrain/draw_38.inc"
};
static Sint16 draw_34_polygons[271] = {
#include "stg04/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[199] = {
#include "stg04/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg04/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg04/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[111] = {
#include "stg04/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[83] = {
#include "stg04/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg04/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg04/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[221] = {
#include "stg04/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[207] = {
#include "stg04/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg04/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg04/terrain/draw_32.inc"
};
static Sint16 draw_30_polygons[235] = {
#include "stg04/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[215] = {
#include "stg04/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg04/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg04/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[235] = {
#include "stg04/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[215] = {
#include "stg04/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg04/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg04/terrain/draw_29.inc"
};
static Sint16 draw_21_polygons[1073] = {
#include "stg04/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[775] = {
#include "stg04/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg04/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg04/terrain/draw_21.inc"
};
static Sint16 draw_1_polygons[799] = {
#include "stg04/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[615] = {
#include "stg04/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg04/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg04/terrain/draw_1.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg04/terrain/collision_1.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg04/terrain/collision_40.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg04/terrain/collision_49.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg04/terrain/collision_53.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg04/terrain/collision_64.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg04/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg04/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg04/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg04/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg04/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg04/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg04/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg04/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg04/terrain/collision_86.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg04/terrain/collision_92.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg04/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg04/terrain/collision_95.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg04/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg04/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg04/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg04/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg04/terrain/collision_100.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg04/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg04/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg04/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg04/terrain/collision_105.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg04/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg04/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg04/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg04/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg04/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg04/terrain/collision_112.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg04/terrain/collision_116.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg04/terrain/collision_118.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg04/terrain/collision_120.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg04/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg04/terrain/collision_123.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg04/terrain/collision_124.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg04/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg04/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg04/terrain/collision_128.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg04/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg04/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg04/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg04/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg04/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg04/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg04/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg04/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg04/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg04/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg04/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg04/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg04/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg04/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg04/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg04/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg04/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg04/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg04/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg04/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg04/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg04/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg04/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg04/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg04/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg04/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg04/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg04/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg04/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg04/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg04/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg04/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg04/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg04/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg04/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg04/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg04/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg04/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg04/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg04/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg04/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg04/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg04/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg04/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg04/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg04/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg04/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg04/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg04/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg04/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg04/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg04/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg04/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg04/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg04/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg04/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg04/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg04/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg04/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg04/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg04/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg04/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg04/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg04/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg04/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg04/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg04/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg04/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg04/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg04/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg04/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg04/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg04/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg04/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg04/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg04/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg04/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg04/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg04/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg04/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg04/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg04/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg04/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg04/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg04/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg04/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg04/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg04/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg04/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg04/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg04/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg04/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg04/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg04/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg04/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg04/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg04/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg04/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg04/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg04/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg04/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg04/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg04/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg04/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg04/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg04/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg04/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg04/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg04/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg04/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg04/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg04/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg04/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg04/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg04/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg04/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg04/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg04/terrain/draw_0.inc"
};
