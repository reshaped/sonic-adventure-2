#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg41/wild_canyon_2p.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char wild_canyon_2p_land_filenames[548] = {
#include "stg41/terrain/wild_canyon_2p_land_filenames.inc"
};

/* STG41.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between wild_canyon_2p.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[178];
struct land_table wild_canyon_2p_land;
static NJS_TEXNAME textures[31];
NJS_TEXLIST wild_canyon_2p_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[5];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[4];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[33];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
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
static Sint16 collision_4_faces[5];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[4];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[110];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[44];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[53];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[26];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[65];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[30];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[60];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[31];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[54];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[27];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[110];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[44];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[125];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[53];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[45];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[20];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[66];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[29];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[60];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[25];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[58];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[25];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[53];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[21];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[50];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[21];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[127];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[46];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[87];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[37];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[15];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[12];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[15];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[12];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[15];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[12];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[15];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[12];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[15];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[12];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[15];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[12];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[208];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[88];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[57];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[23];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[54];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[23];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[42];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[22];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[81];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[34];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[41];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[18];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[69];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[34];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[107];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[42];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[27];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[16];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[42];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[21];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[94];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[43];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[38];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[20];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[47];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[24];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[63];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[24];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[42];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[22];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[19];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[11];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[12];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[8];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[111];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[56];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[111];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[56];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[5];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[4];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[5];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[4];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[37];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[12];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[37];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[12];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[37];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[12];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[37];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[12];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[37];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[12];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[37];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[12];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[25];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[12];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[35];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[16];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[35];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[16];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[186];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[50];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[186];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[50];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[182];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[50];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[182];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[50];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[30];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[18];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[11];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[8];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[14];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[8];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[14];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[8];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[14];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[8];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[16];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[8];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[14];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[8];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[35];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[16];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[32];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[16];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[34];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[16];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[9];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[8];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[14];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[8];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[14];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[8];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[77];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[42];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[167];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[47];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[11];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[8];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static Sint16 draw_83_polygons[357];
static Sint32 draw_83_vertices[203];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_84_polygons[357];
static Sint32 draw_84_vertices[195];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_85_polygons[357];
static Sint32 draw_85_vertices[199];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_86_polygons[357];
static Sint32 draw_86_vertices[195];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_87_polygons[303];
static Sint32 draw_87_vertices[171];
static NJS_CNK_MODEL draw_87_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_88_polygons[365];
static Sint32 draw_88_vertices[195];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_89_polygons[365];
static Sint32 draw_89_vertices[195];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_90_polygons[191];
static Sint32 draw_90_vertices[127];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_91_polygons[79];
static Sint32 draw_91_vertices[43];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_92_polygons[55];
static Sint32 draw_92_vertices[51];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_93_polygons[139];
static Sint32 draw_93_vertices[99];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_94_polygons[81];
static Sint32 draw_94_vertices[51];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_95_polygons[91];
static Sint32 draw_95_vertices[51];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_96_polygons[91];
static Sint32 draw_96_vertices[51];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_97_polygons[239];
static Sint32 draw_97_vertices[203];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_98_polygons[247];
static Sint32 draw_98_vertices[147];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_99_polygons[71];
static Sint32 draw_99_vertices[51];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_100_polygons[71];
static Sint32 draw_100_vertices[51];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_101_polygons[79];
static Sint32 draw_101_vertices[59];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_102_polygons[177];
static Sint32 draw_102_vertices[87];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_103_polygons[85];
static Sint32 draw_103_vertices[59];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_104_polygons[119];
static Sint32 draw_104_vertices[67];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_105_polygons[79];
static Sint32 draw_105_vertices[59];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_106_polygons[79];
static Sint32 draw_106_vertices[59];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_107_polygons[215];
static Sint32 draw_107_vertices[151];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_108_polygons[215];
static Sint32 draw_108_vertices[151];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_109_polygons[39];
static Sint32 draw_109_vertices[35];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_110_polygons[39];
static Sint32 draw_110_vertices[35];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_111_polygons[73];
static Sint32 draw_111_vertices[51];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_112_polygons[71];
static Sint32 draw_112_vertices[51];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_113_polygons[43];
static Sint32 draw_113_vertices[27];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_114_polygons[43];
static Sint32 draw_114_vertices[27];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_115_polygons[43];
static Sint32 draw_115_vertices[27];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_116_polygons[165];
static Sint32 draw_116_vertices[127];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_117_polygons[225];
static Sint32 draw_117_vertices[131];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_118_polygons[219];
static Sint32 draw_118_vertices[123];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_119_polygons[211];
static Sint32 draw_119_vertices[103];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_120_polygons[177];
static Sint32 draw_120_vertices[115];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_121_polygons[333];
static Sint32 draw_121_vertices[207];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_122_polygons[1089];
static Sint32 draw_122_vertices[575];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_123_polygons[1091];
static Sint32 draw_123_vertices[555];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_124_polygons[1083];
static Sint32 draw_124_vertices[527];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_125_polygons[1089];
static Sint32 draw_125_vertices[531];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_126_polygons[1119];
static Sint32 draw_126_vertices[615];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_127_polygons[1111];
static Sint32 draw_127_vertices[579];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_128_polygons[1119];
static Sint32 draw_128_vertices[615];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_129_polygons[1133];
static Sint32 draw_129_vertices[587];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_130_polygons[1111];
static Sint32 draw_130_vertices[607];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_131_polygons[139];
static Sint32 draw_131_vertices[99];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_132_polygons[69];
static Sint32 draw_132_vertices[51];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_133_polygons[639];
static Sint32 draw_133_vertices[395];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_134_polygons[37];
static Sint32 draw_134_vertices[35];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_135_polygons[37];
static Sint32 draw_135_vertices[35];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_136_polygons[115];
static Sint32 draw_136_vertices[59];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_137_polygons[115];
static Sint32 draw_137_vertices[67];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_138_polygons[51];
static Sint32 draw_138_vertices[35];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_139_polygons[57];
static Sint32 draw_139_vertices[43];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_140_polygons[237];
static Sint32 draw_140_vertices[143];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_141_polygons[245];
static Sint32 draw_141_vertices[123];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_142_polygons[245];
static Sint32 draw_142_vertices[123];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_143_polygons[245];
static Sint32 draw_143_vertices[123];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_144_polygons[245];
static Sint32 draw_144_vertices[123];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_145_polygons[581];
static Sint32 draw_145_vertices[387];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_146_polygons[187];
static Sint32 draw_146_vertices[143];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_147_polygons[107];
static Sint32 draw_147_vertices[119];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_148_polygons[589];
static Sint32 draw_148_vertices[303];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_149_polygons[69];
static Sint32 draw_149_vertices[55];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_150_polygons[219];
static Sint32 draw_150_vertices[123];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_151_polygons[69];
static Sint32 draw_151_vertices[47];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_152_polygons[113];
static Sint32 draw_152_vertices[83];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_153_polygons[69];
static Sint32 draw_153_vertices[59];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_154_polygons[89];
static Sint32 draw_154_vertices[59];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_155_polygons[441];
static Sint32 draw_155_vertices[263];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_156_polygons[677];
static Sint32 draw_156_vertices[411];
static NJS_CNK_MODEL draw_156_model;
static NJS_CNK_OBJECT draw_156;
static Sint16 draw_157_polygons[31];
static Sint32 draw_157_vertices[27];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_157;
static Sint16 draw_158_polygons[25];
static Sint32 draw_158_vertices[19];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_159_polygons[235];
static Sint32 draw_159_vertices[191];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_160_polygons[235];
static Sint32 draw_160_vertices[195];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_160;
static Sint16 draw_161_polygons[235];
static Sint32 draw_161_vertices[195];
static NJS_CNK_MODEL draw_161_model;
static NJS_CNK_OBJECT draw_161;
static Sint16 draw_162_polygons[101];
static Sint32 draw_162_vertices[119];
static NJS_CNK_MODEL draw_162_model;
static NJS_CNK_OBJECT draw_162;
static Sint16 draw_163_polygons[75];
static Sint32 draw_163_vertices[63];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_163;
static Sint16 draw_164_polygons[75];
static Sint32 draw_164_vertices[59];
static NJS_CNK_MODEL draw_164_model;
static NJS_CNK_OBJECT draw_164;
static Sint16 draw_165_polygons[75];
static Sint32 draw_165_vertices[63];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_166_polygons[57];
static Sint32 draw_166_vertices[43];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_167_polygons[291];
static Sint32 draw_167_vertices[163];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_168_polygons[57];
static Sint32 draw_168_vertices[55];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_169_polygons[57];
static Sint32 draw_169_vertices[47];
static NJS_CNK_MODEL draw_169_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_170_polygons[57];
static Sint32 draw_170_vertices[55];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_170;
static Sint16 draw_171_polygons[69];
static Sint32 draw_171_vertices[47];
static NJS_CNK_MODEL draw_171_model;
static NJS_CNK_OBJECT draw_171;
static Sint16 draw_172_polygons[415];
static Sint32 draw_172_vertices[247];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_172;
static Sint16 draw_173_polygons[401];
static Sint32 draw_173_vertices[219];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_174_polygons[105];
static Sint32 draw_174_vertices[75];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_175_polygons[91];
static Sint32 draw_175_vertices[51];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_175;
static Sint16 draw_176_polygons[239];
static Sint32 draw_176_vertices[147];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_176;
static Sint16 draw_177_polygons[247];
static Sint32 draw_177_vertices[163];
static NJS_CNK_MODEL draw_177_model;
static NJS_CNK_OBJECT draw_177;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;

static struct land_entry entries[178] = {
#include "stg41/terrain/entries.inc"
};
struct land_table wild_canyon_2p_land = {
#include "stg41/terrain/wild_canyon_2p_land.inc"
};
static NJS_TEXNAME textures[31] = {
#include "stg41/terrain/textures.inc"
};
NJS_TEXLIST wild_canyon_2p_land_texlist = {
#include "stg41/terrain/wild_canyon_2p_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg41/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[5] = {
#include "stg41/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg41/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[4] = {
#include "stg41/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg41/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg41/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg41/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[33] = {
#include "stg41/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg41/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg41/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg41/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg41/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg41/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[5] = {
#include "stg41/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg41/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[4] = {
#include "stg41/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg41/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg41/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg41/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[5] = {
#include "stg41/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg41/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[4] = {
#include "stg41/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg41/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg41/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg41/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[5] = {
#include "stg41/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg41/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[4] = {
#include "stg41/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg41/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg41/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg41/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[110] = {
#include "stg41/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg41/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[44] = {
#include "stg41/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg41/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg41/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg41/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[53] = {
#include "stg41/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg41/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[26] = {
#include "stg41/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg41/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg41/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg41/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[65] = {
#include "stg41/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg41/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[30] = {
#include "stg41/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg41/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg41/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg41/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[60] = {
#include "stg41/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg41/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[31] = {
#include "stg41/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg41/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg41/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg41/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[54] = {
#include "stg41/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg41/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[27] = {
#include "stg41/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg41/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg41/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg41/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[110] = {
#include "stg41/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg41/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[44] = {
#include "stg41/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg41/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg41/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg41/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[125] = {
#include "stg41/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg41/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[53] = {
#include "stg41/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg41/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg41/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg41/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[45] = {
#include "stg41/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg41/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[20] = {
#include "stg41/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg41/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg41/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg41/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[66] = {
#include "stg41/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg41/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[29] = {
#include "stg41/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg41/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg41/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg41/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[60] = {
#include "stg41/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg41/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[25] = {
#include "stg41/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg41/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg41/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg41/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[58] = {
#include "stg41/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg41/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[25] = {
#include "stg41/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg41/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg41/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg41/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[53] = {
#include "stg41/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg41/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[21] = {
#include "stg41/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg41/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg41/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg41/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[50] = {
#include "stg41/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg41/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[21] = {
#include "stg41/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg41/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg41/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg41/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[127] = {
#include "stg41/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg41/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[46] = {
#include "stg41/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg41/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg41/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg41/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[87] = {
#include "stg41/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg41/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[37] = {
#include "stg41/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg41/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg41/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg41/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[15] = {
#include "stg41/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg41/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[12] = {
#include "stg41/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg41/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg41/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg41/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[15] = {
#include "stg41/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg41/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[12] = {
#include "stg41/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg41/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg41/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg41/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[15] = {
#include "stg41/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg41/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[12] = {
#include "stg41/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg41/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg41/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg41/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[15] = {
#include "stg41/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg41/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[12] = {
#include "stg41/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg41/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg41/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg41/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[15] = {
#include "stg41/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg41/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[12] = {
#include "stg41/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg41/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg41/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg41/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[15] = {
#include "stg41/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg41/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[12] = {
#include "stg41/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg41/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg41/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg41/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[208] = {
#include "stg41/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg41/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[88] = {
#include "stg41/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg41/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg41/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg41/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[57] = {
#include "stg41/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg41/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[23] = {
#include "stg41/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg41/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg41/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg41/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[54] = {
#include "stg41/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg41/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[23] = {
#include "stg41/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg41/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg41/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg41/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[42] = {
#include "stg41/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg41/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[22] = {
#include "stg41/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg41/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg41/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg41/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[81] = {
#include "stg41/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg41/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[34] = {
#include "stg41/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg41/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg41/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg41/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[41] = {
#include "stg41/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg41/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[18] = {
#include "stg41/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg41/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg41/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg41/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[69] = {
#include "stg41/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg41/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[34] = {
#include "stg41/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg41/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg41/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg41/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[107] = {
#include "stg41/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg41/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[42] = {
#include "stg41/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg41/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg41/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg41/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[27] = {
#include "stg41/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg41/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[16] = {
#include "stg41/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg41/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg41/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg41/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[42] = {
#include "stg41/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg41/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[21] = {
#include "stg41/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg41/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg41/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg41/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[94] = {
#include "stg41/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg41/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[43] = {
#include "stg41/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg41/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg41/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg41/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[38] = {
#include "stg41/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg41/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[20] = {
#include "stg41/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg41/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg41/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg41/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[47] = {
#include "stg41/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg41/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[24] = {
#include "stg41/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg41/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg41/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg41/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[63] = {
#include "stg41/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg41/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[24] = {
#include "stg41/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg41/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg41/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg41/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[42] = {
#include "stg41/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg41/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[22] = {
#include "stg41/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg41/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg41/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg41/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[19] = {
#include "stg41/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg41/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[11] = {
#include "stg41/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg41/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg41/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg41/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[12] = {
#include "stg41/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg41/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[8] = {
#include "stg41/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg41/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg41/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg41/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[111] = {
#include "stg41/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg41/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[56] = {
#include "stg41/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg41/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg41/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg41/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[111] = {
#include "stg41/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg41/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[56] = {
#include "stg41/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg41/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg41/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg41/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[5] = {
#include "stg41/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg41/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[4] = {
#include "stg41/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg41/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg41/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg41/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[5] = {
#include "stg41/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg41/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[4] = {
#include "stg41/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg41/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg41/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg41/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[37] = {
#include "stg41/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg41/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[12] = {
#include "stg41/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg41/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg41/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg41/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[37] = {
#include "stg41/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg41/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[12] = {
#include "stg41/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg41/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg41/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg41/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[37] = {
#include "stg41/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg41/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[12] = {
#include "stg41/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg41/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg41/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg41/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[37] = {
#include "stg41/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg41/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[12] = {
#include "stg41/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg41/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg41/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg41/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[37] = {
#include "stg41/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg41/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[12] = {
#include "stg41/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg41/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg41/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg41/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[37] = {
#include "stg41/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg41/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[12] = {
#include "stg41/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg41/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg41/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg41/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[25] = {
#include "stg41/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg41/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[12] = {
#include "stg41/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg41/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg41/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg41/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[35] = {
#include "stg41/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg41/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[16] = {
#include "stg41/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg41/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg41/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg41/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[35] = {
#include "stg41/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg41/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[16] = {
#include "stg41/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg41/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg41/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg41/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[186] = {
#include "stg41/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg41/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[50] = {
#include "stg41/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg41/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg41/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg41/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[186] = {
#include "stg41/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg41/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[50] = {
#include "stg41/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg41/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg41/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg41/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[182] = {
#include "stg41/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg41/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[50] = {
#include "stg41/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg41/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg41/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg41/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[182] = {
#include "stg41/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg41/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[50] = {
#include "stg41/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg41/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg41/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg41/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[30] = {
#include "stg41/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg41/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[18] = {
#include "stg41/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg41/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg41/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg41/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[11] = {
#include "stg41/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg41/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[8] = {
#include "stg41/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg41/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg41/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg41/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[14] = {
#include "stg41/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg41/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[8] = {
#include "stg41/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg41/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg41/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg41/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[14] = {
#include "stg41/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg41/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[8] = {
#include "stg41/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg41/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg41/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg41/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[14] = {
#include "stg41/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg41/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[8] = {
#include "stg41/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg41/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg41/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg41/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[16] = {
#include "stg41/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg41/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[8] = {
#include "stg41/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg41/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg41/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg41/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[14] = {
#include "stg41/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg41/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[8] = {
#include "stg41/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg41/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg41/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg41/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[35] = {
#include "stg41/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg41/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[16] = {
#include "stg41/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg41/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg41/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg41/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[32] = {
#include "stg41/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg41/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[16] = {
#include "stg41/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg41/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg41/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg41/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[34] = {
#include "stg41/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg41/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[16] = {
#include "stg41/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg41/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg41/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg41/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[9] = {
#include "stg41/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg41/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[8] = {
#include "stg41/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg41/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg41/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg41/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[14] = {
#include "stg41/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg41/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[8] = {
#include "stg41/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg41/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg41/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg41/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[14] = {
#include "stg41/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg41/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[8] = {
#include "stg41/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg41/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg41/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg41/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[77] = {
#include "stg41/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg41/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[42] = {
#include "stg41/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg41/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg41/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg41/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[167] = {
#include "stg41/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg41/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[47] = {
#include "stg41/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg41/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg41/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg41/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[11] = {
#include "stg41/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg41/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[8] = {
#include "stg41/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg41/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg41/terrain/collision_82.inc"
};
static Sint16 draw_83_polygons[357] = {
#include "stg41/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[203] = {
#include "stg41/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg41/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg41/terrain/draw_83.inc"
};
static Sint16 draw_84_polygons[357] = {
#include "stg41/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[195] = {
#include "stg41/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg41/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg41/terrain/draw_84.inc"
};
static Sint16 draw_85_polygons[357] = {
#include "stg41/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[199] = {
#include "stg41/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg41/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg41/terrain/draw_85.inc"
};
static Sint16 draw_86_polygons[357] = {
#include "stg41/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[195] = {
#include "stg41/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg41/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg41/terrain/draw_86.inc"
};
static Sint16 draw_87_polygons[303] = {
#include "stg41/terrain/draw_87_polygons.inc"
};
static Sint32 draw_87_vertices[171] = {
#include "stg41/terrain/draw_87_vertices.inc"
};
static NJS_CNK_MODEL draw_87_model = {
#include "stg41/terrain/draw_87_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg41/terrain/draw_87.inc"
};
static Sint16 draw_88_polygons[365] = {
#include "stg41/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[195] = {
#include "stg41/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg41/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg41/terrain/draw_88.inc"
};
static Sint16 draw_89_polygons[365] = {
#include "stg41/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[195] = {
#include "stg41/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg41/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg41/terrain/draw_89.inc"
};
static Sint16 draw_90_polygons[191] = {
#include "stg41/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[127] = {
#include "stg41/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg41/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg41/terrain/draw_90.inc"
};
static Sint16 draw_91_polygons[79] = {
#include "stg41/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[43] = {
#include "stg41/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg41/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg41/terrain/draw_91.inc"
};
static Sint16 draw_92_polygons[55] = {
#include "stg41/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[51] = {
#include "stg41/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg41/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg41/terrain/draw_92.inc"
};
static Sint16 draw_93_polygons[139] = {
#include "stg41/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[99] = {
#include "stg41/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg41/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg41/terrain/draw_93.inc"
};
static Sint16 draw_94_polygons[81] = {
#include "stg41/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[51] = {
#include "stg41/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg41/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg41/terrain/draw_94.inc"
};
static Sint16 draw_95_polygons[91] = {
#include "stg41/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[51] = {
#include "stg41/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg41/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg41/terrain/draw_95.inc"
};
static Sint16 draw_96_polygons[91] = {
#include "stg41/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[51] = {
#include "stg41/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg41/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg41/terrain/draw_96.inc"
};
static Sint16 draw_97_polygons[239] = {
#include "stg41/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[203] = {
#include "stg41/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg41/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg41/terrain/draw_97.inc"
};
static Sint16 draw_98_polygons[247] = {
#include "stg41/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[147] = {
#include "stg41/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg41/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg41/terrain/draw_98.inc"
};
static Sint16 draw_99_polygons[71] = {
#include "stg41/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[51] = {
#include "stg41/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg41/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg41/terrain/draw_99.inc"
};
static Sint16 draw_100_polygons[71] = {
#include "stg41/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[51] = {
#include "stg41/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg41/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg41/terrain/draw_100.inc"
};
static Sint16 draw_101_polygons[79] = {
#include "stg41/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[59] = {
#include "stg41/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg41/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg41/terrain/draw_101.inc"
};
static Sint16 draw_102_polygons[177] = {
#include "stg41/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[87] = {
#include "stg41/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg41/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg41/terrain/draw_102.inc"
};
static Sint16 draw_103_polygons[85] = {
#include "stg41/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[59] = {
#include "stg41/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg41/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg41/terrain/draw_103.inc"
};
static Sint16 draw_104_polygons[119] = {
#include "stg41/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[67] = {
#include "stg41/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg41/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg41/terrain/draw_104.inc"
};
static Sint16 draw_105_polygons[79] = {
#include "stg41/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[59] = {
#include "stg41/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg41/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg41/terrain/draw_105.inc"
};
static Sint16 draw_106_polygons[79] = {
#include "stg41/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[59] = {
#include "stg41/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg41/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg41/terrain/draw_106.inc"
};
static Sint16 draw_107_polygons[215] = {
#include "stg41/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[151] = {
#include "stg41/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg41/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg41/terrain/draw_107.inc"
};
static Sint16 draw_108_polygons[215] = {
#include "stg41/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[151] = {
#include "stg41/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg41/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg41/terrain/draw_108.inc"
};
static Sint16 draw_109_polygons[39] = {
#include "stg41/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[35] = {
#include "stg41/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg41/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg41/terrain/draw_109.inc"
};
static Sint16 draw_110_polygons[39] = {
#include "stg41/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[35] = {
#include "stg41/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg41/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg41/terrain/draw_110.inc"
};
static Sint16 draw_111_polygons[73] = {
#include "stg41/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[51] = {
#include "stg41/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg41/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg41/terrain/draw_111.inc"
};
static Sint16 draw_112_polygons[71] = {
#include "stg41/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[51] = {
#include "stg41/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg41/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg41/terrain/draw_112.inc"
};
static Sint16 draw_113_polygons[43] = {
#include "stg41/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[27] = {
#include "stg41/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg41/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg41/terrain/draw_113.inc"
};
static Sint16 draw_114_polygons[43] = {
#include "stg41/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[27] = {
#include "stg41/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg41/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg41/terrain/draw_114.inc"
};
static Sint16 draw_115_polygons[43] = {
#include "stg41/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[27] = {
#include "stg41/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg41/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg41/terrain/draw_115.inc"
};
static Sint16 draw_116_polygons[165] = {
#include "stg41/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[127] = {
#include "stg41/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg41/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg41/terrain/draw_116.inc"
};
static Sint16 draw_117_polygons[225] = {
#include "stg41/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[131] = {
#include "stg41/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg41/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg41/terrain/draw_117.inc"
};
static Sint16 draw_118_polygons[219] = {
#include "stg41/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[123] = {
#include "stg41/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg41/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg41/terrain/draw_118.inc"
};
static Sint16 draw_119_polygons[211] = {
#include "stg41/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[103] = {
#include "stg41/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg41/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg41/terrain/draw_119.inc"
};
static Sint16 draw_120_polygons[177] = {
#include "stg41/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[115] = {
#include "stg41/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg41/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg41/terrain/draw_120.inc"
};
static Sint16 draw_121_polygons[333] = {
#include "stg41/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[207] = {
#include "stg41/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg41/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg41/terrain/draw_121.inc"
};
static Sint16 draw_122_polygons[1089] = {
#include "stg41/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[575] = {
#include "stg41/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg41/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg41/terrain/draw_122.inc"
};
static Sint16 draw_123_polygons[1091] = {
#include "stg41/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[555] = {
#include "stg41/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg41/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg41/terrain/draw_123.inc"
};
static Sint16 draw_124_polygons[1083] = {
#include "stg41/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[527] = {
#include "stg41/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg41/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg41/terrain/draw_124.inc"
};
static Sint16 draw_125_polygons[1089] = {
#include "stg41/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[531] = {
#include "stg41/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg41/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg41/terrain/draw_125.inc"
};
static Sint16 draw_126_polygons[1119] = {
#include "stg41/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[615] = {
#include "stg41/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg41/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg41/terrain/draw_126.inc"
};
static Sint16 draw_127_polygons[1111] = {
#include "stg41/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[579] = {
#include "stg41/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg41/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg41/terrain/draw_127.inc"
};
static Sint16 draw_128_polygons[1119] = {
#include "stg41/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[615] = {
#include "stg41/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg41/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg41/terrain/draw_128.inc"
};
static Sint16 draw_129_polygons[1133] = {
#include "stg41/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[587] = {
#include "stg41/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg41/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg41/terrain/draw_129.inc"
};
static Sint16 draw_130_polygons[1111] = {
#include "stg41/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[607] = {
#include "stg41/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg41/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg41/terrain/draw_130.inc"
};
static Sint16 draw_131_polygons[139] = {
#include "stg41/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[99] = {
#include "stg41/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg41/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg41/terrain/draw_131.inc"
};
static Sint16 draw_132_polygons[69] = {
#include "stg41/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[51] = {
#include "stg41/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg41/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg41/terrain/draw_132.inc"
};
static Sint16 draw_133_polygons[639] = {
#include "stg41/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[395] = {
#include "stg41/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg41/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg41/terrain/draw_133.inc"
};
static Sint16 draw_134_polygons[37] = {
#include "stg41/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[35] = {
#include "stg41/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg41/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg41/terrain/draw_134.inc"
};
static Sint16 draw_135_polygons[37] = {
#include "stg41/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[35] = {
#include "stg41/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg41/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg41/terrain/draw_135.inc"
};
static Sint16 draw_136_polygons[115] = {
#include "stg41/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[59] = {
#include "stg41/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg41/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg41/terrain/draw_136.inc"
};
static Sint16 draw_137_polygons[115] = {
#include "stg41/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[67] = {
#include "stg41/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg41/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg41/terrain/draw_137.inc"
};
static Sint16 draw_138_polygons[51] = {
#include "stg41/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[35] = {
#include "stg41/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg41/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg41/terrain/draw_138.inc"
};
static Sint16 draw_139_polygons[57] = {
#include "stg41/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[43] = {
#include "stg41/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg41/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg41/terrain/draw_139.inc"
};
static Sint16 draw_140_polygons[237] = {
#include "stg41/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[143] = {
#include "stg41/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg41/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg41/terrain/draw_140.inc"
};
static Sint16 draw_141_polygons[245] = {
#include "stg41/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[123] = {
#include "stg41/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg41/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg41/terrain/draw_141.inc"
};
static Sint16 draw_142_polygons[245] = {
#include "stg41/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[123] = {
#include "stg41/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg41/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg41/terrain/draw_142.inc"
};
static Sint16 draw_143_polygons[245] = {
#include "stg41/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[123] = {
#include "stg41/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg41/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg41/terrain/draw_143.inc"
};
static Sint16 draw_144_polygons[245] = {
#include "stg41/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[123] = {
#include "stg41/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg41/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg41/terrain/draw_144.inc"
};
static Sint16 draw_145_polygons[581] = {
#include "stg41/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[387] = {
#include "stg41/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg41/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg41/terrain/draw_145.inc"
};
static Sint16 draw_146_polygons[187] = {
#include "stg41/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[143] = {
#include "stg41/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg41/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg41/terrain/draw_146.inc"
};
static Sint16 draw_147_polygons[107] = {
#include "stg41/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[119] = {
#include "stg41/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg41/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg41/terrain/draw_147.inc"
};
static Sint16 draw_148_polygons[589] = {
#include "stg41/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[303] = {
#include "stg41/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg41/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg41/terrain/draw_148.inc"
};
static Sint16 draw_149_polygons[69] = {
#include "stg41/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[55] = {
#include "stg41/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg41/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg41/terrain/draw_149.inc"
};
static Sint16 draw_150_polygons[219] = {
#include "stg41/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[123] = {
#include "stg41/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg41/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg41/terrain/draw_150.inc"
};
static Sint16 draw_151_polygons[69] = {
#include "stg41/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[47] = {
#include "stg41/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg41/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg41/terrain/draw_151.inc"
};
static Sint16 draw_152_polygons[113] = {
#include "stg41/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[83] = {
#include "stg41/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg41/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg41/terrain/draw_152.inc"
};
static Sint16 draw_153_polygons[69] = {
#include "stg41/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[59] = {
#include "stg41/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg41/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg41/terrain/draw_153.inc"
};
static Sint16 draw_154_polygons[89] = {
#include "stg41/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[59] = {
#include "stg41/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg41/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg41/terrain/draw_154.inc"
};
static Sint16 draw_155_polygons[441] = {
#include "stg41/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[263] = {
#include "stg41/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg41/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg41/terrain/draw_155.inc"
};
static Sint16 draw_156_polygons[677] = {
#include "stg41/terrain/draw_156_polygons.inc"
};
static Sint32 draw_156_vertices[411] = {
#include "stg41/terrain/draw_156_vertices.inc"
};
static NJS_CNK_MODEL draw_156_model = {
#include "stg41/terrain/draw_156_model.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg41/terrain/draw_156.inc"
};
static Sint16 draw_157_polygons[31] = {
#include "stg41/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[27] = {
#include "stg41/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg41/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg41/terrain/draw_157.inc"
};
static Sint16 draw_158_polygons[25] = {
#include "stg41/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[19] = {
#include "stg41/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg41/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg41/terrain/draw_158.inc"
};
static Sint16 draw_159_polygons[235] = {
#include "stg41/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[191] = {
#include "stg41/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg41/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg41/terrain/draw_159.inc"
};
static Sint16 draw_160_polygons[235] = {
#include "stg41/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[195] = {
#include "stg41/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg41/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg41/terrain/draw_160.inc"
};
static Sint16 draw_161_polygons[235] = {
#include "stg41/terrain/draw_161_polygons.inc"
};
static Sint32 draw_161_vertices[195] = {
#include "stg41/terrain/draw_161_vertices.inc"
};
static NJS_CNK_MODEL draw_161_model = {
#include "stg41/terrain/draw_161_model.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg41/terrain/draw_161.inc"
};
static Sint16 draw_162_polygons[101] = {
#include "stg41/terrain/draw_162_polygons.inc"
};
static Sint32 draw_162_vertices[119] = {
#include "stg41/terrain/draw_162_vertices.inc"
};
static NJS_CNK_MODEL draw_162_model = {
#include "stg41/terrain/draw_162_model.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg41/terrain/draw_162.inc"
};
static Sint16 draw_163_polygons[75] = {
#include "stg41/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[63] = {
#include "stg41/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg41/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg41/terrain/draw_163.inc"
};
static Sint16 draw_164_polygons[75] = {
#include "stg41/terrain/draw_164_polygons.inc"
};
static Sint32 draw_164_vertices[59] = {
#include "stg41/terrain/draw_164_vertices.inc"
};
static NJS_CNK_MODEL draw_164_model = {
#include "stg41/terrain/draw_164_model.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg41/terrain/draw_164.inc"
};
static Sint16 draw_165_polygons[75] = {
#include "stg41/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[63] = {
#include "stg41/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg41/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg41/terrain/draw_165.inc"
};
static Sint16 draw_166_polygons[57] = {
#include "stg41/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[43] = {
#include "stg41/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg41/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg41/terrain/draw_166.inc"
};
static Sint16 draw_167_polygons[291] = {
#include "stg41/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[163] = {
#include "stg41/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg41/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg41/terrain/draw_167.inc"
};
static Sint16 draw_168_polygons[57] = {
#include "stg41/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[55] = {
#include "stg41/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg41/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg41/terrain/draw_168.inc"
};
static Sint16 draw_169_polygons[57] = {
#include "stg41/terrain/draw_169_polygons.inc"
};
static Sint32 draw_169_vertices[47] = {
#include "stg41/terrain/draw_169_vertices.inc"
};
static NJS_CNK_MODEL draw_169_model = {
#include "stg41/terrain/draw_169_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg41/terrain/draw_169.inc"
};
static Sint16 draw_170_polygons[57] = {
#include "stg41/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[55] = {
#include "stg41/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg41/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg41/terrain/draw_170.inc"
};
static Sint16 draw_171_polygons[69] = {
#include "stg41/terrain/draw_171_polygons.inc"
};
static Sint32 draw_171_vertices[47] = {
#include "stg41/terrain/draw_171_vertices.inc"
};
static NJS_CNK_MODEL draw_171_model = {
#include "stg41/terrain/draw_171_model.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg41/terrain/draw_171.inc"
};
static Sint16 draw_172_polygons[415] = {
#include "stg41/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[247] = {
#include "stg41/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg41/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg41/terrain/draw_172.inc"
};
static Sint16 draw_173_polygons[401] = {
#include "stg41/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[219] = {
#include "stg41/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg41/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg41/terrain/draw_173.inc"
};
static Sint16 draw_174_polygons[105] = {
#include "stg41/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[75] = {
#include "stg41/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg41/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg41/terrain/draw_174.inc"
};
static Sint16 draw_175_polygons[91] = {
#include "stg41/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[51] = {
#include "stg41/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg41/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg41/terrain/draw_175.inc"
};
static Sint16 draw_176_polygons[239] = {
#include "stg41/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[147] = {
#include "stg41/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg41/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg41/terrain/draw_176.inc"
};
static Sint16 draw_177_polygons[247] = {
#include "stg41/terrain/draw_177_polygons.inc"
};
static Sint32 draw_177_vertices[163] = {
#include "stg41/terrain/draw_177_vertices.inc"
};
static NJS_CNK_MODEL draw_177_model = {
#include "stg41/terrain/draw_177_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg41/terrain/draw_177.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg41/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg41/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg41/terrain/collision_48.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg41/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg41/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg41/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg41/terrain/collision_81.inc"
};
