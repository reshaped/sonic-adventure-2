#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg03/green_forest.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char green_forest_land_filenames[545] = {
#include "stg03/terrain/green_forest_land_filenames.inc"
};

/* STG03.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between green_forest.c and object_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[480];
struct land_table green_forest_land;
static NJS_TEXNAME textures[38];
NJS_TEXLIST green_forest_ground_texlist;
NJS_TEXLIST green_forest_water_texlist;
NJS_TEXLIST green_forest_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[53];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[28];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[53];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[27];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[135];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[78];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[24];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[16];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[25];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[14];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[37];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[24];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[61];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[29];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[70];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[37];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[63];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[36];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[29];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[16];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[95];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[48];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[54];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[29];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[118];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[55];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[51];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[32];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[28];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[15];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[132];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[68];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[31];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[16];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[9];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[8];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[12];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[8];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[12];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[54];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[39];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[55];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[30];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[65];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[36];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[26];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[18];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[36];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[24];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[36];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[22];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[19];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[18];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[14];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[9];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[60];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[32];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[37];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[22];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[150];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[84];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[28];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[15];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[111];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[50];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[104];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[54];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[24];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[16];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[47];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[23];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[52];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[22];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[5];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[4];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[51];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[26];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[105];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[60];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[29];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[18];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[77];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[44];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[272];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[100];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[49];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[22];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[32];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[17];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[567];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[310];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[78];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[36];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[77];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[30];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[135];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[53];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[43];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[22];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[567];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[310];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[103];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[58];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[50];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[26];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[153];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[74];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[72];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[32];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[45];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[24];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[92];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[39];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[124];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[75];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[31];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[16];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[42];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[30];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[53];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[28];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[96];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[43];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[91];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[48];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[56];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[25];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[103];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[58];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[82];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[41];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[51];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[29];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[64];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[29];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[79];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[38];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[79];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[47];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[190];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[81];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[190];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[89];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[155];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[75];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[49];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[20];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[50];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[25];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[97];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[42];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[43];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[27];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[297];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[160];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[15];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[11];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[18];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[11];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[189];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[91];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[11];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[10];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[13];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[12];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[30];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[18];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[51];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[24];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[9];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[8];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[35];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[22];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[172];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[68];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[245];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[97];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[617];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[338];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[114];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[76];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[26];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[18];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[45];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[24];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[22];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[15];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_109_materials[1];
static Sint16 collision_109_faces[26];
static NJS_MESHSET collision_109_meshsets[1];
static NJS_POINT3 collision_109_points[18];
static NJS_MODEL collision_109_model;
static NJS_OBJECT collision_109;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[45];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[24];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[43];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[23];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[26];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[18];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[18];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[12];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[18];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[12];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[27];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[17];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[30];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[18];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[68];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[32];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[222];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[99];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[25];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[18];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[22];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[15];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[65];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[30];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[205];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[87];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_126_materials[1];
static Sint16 collision_126_faces[55];
static NJS_MESHSET collision_126_meshsets[1];
static NJS_POINT3 collision_126_points[40];
static NJS_MODEL collision_126_model;
static NJS_OBJECT collision_126;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[136];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[54];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_128_materials[1];
static Sint16 collision_128_faces[67];
static NJS_MESHSET collision_128_meshsets[1];
static NJS_POINT3 collision_128_points[30];
static NJS_MODEL collision_128_model;
static NJS_OBJECT collision_128;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[25];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[14];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[9];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[8];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[51];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[21];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_132_materials[1];
static Sint16 collision_132_faces[45];
static NJS_MESHSET collision_132_meshsets[1];
static NJS_POINT3 collision_132_points[23];
static NJS_MODEL collision_132_model;
static NJS_OBJECT collision_132;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[20];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[9];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[26];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[11];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[69];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[34];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[15];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[7];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[83];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[44];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[186];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[72];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[63];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[40];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[25];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[18];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[9];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[8];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[21];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[20];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[21];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[20];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[111];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[60];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[112];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[50];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[117];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[60];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[341];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[184];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[366];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[183];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[119];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[53];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[399];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[210];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[115];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[62];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[323];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[169];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[64];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[26];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[387];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[210];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[84];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[50];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[43];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[26];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[63];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[40];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[5];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[4];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[7];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[6];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static Sint16 draw_40_polygons[221];
static Sint32 draw_40_vertices[207];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_293;
static Sint16 draw_15_polygons[69];
static Sint32 draw_15_vertices[51];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_292;
static Sint16 draw_16_polygons[235];
static Sint32 draw_16_vertices[215];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_290;
static Sint16 draw_289_polygons[133];
static Sint32 draw_289_vertices[123];
static NJS_CNK_MODEL draw_289_model;
static NJS_CNK_OBJECT draw_289;
static Sint16 draw_288_polygons[139];
static Sint32 draw_288_vertices[111];
static NJS_CNK_MODEL draw_288_model;
static NJS_CNK_OBJECT draw_288;
static Sint16 draw_37_polygons[499];
static Sint32 draw_37_vertices[387];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_287;
static Sint16 draw_33_polygons[499];
static Sint32 draw_33_vertices[395];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_286;
static Sint16 draw_285_polygons[633];
static Sint32 draw_285_vertices[507];
static NJS_CNK_MODEL draw_285_model;
static NJS_CNK_OBJECT draw_285;
static Sint16 draw_21_polygons[129];
static Sint32 draw_21_vertices[99];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_284;
static Sint16 draw_19_polygons[961];
static Sint32 draw_19_vertices[999];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_282;
static Sint16 draw_280_polygons[43];
static Sint32 draw_280_vertices[31];
static NJS_CNK_MODEL draw_280_model;
static NJS_CNK_OBJECT draw_280;
static Sint16 draw_279_polygons[415];
static Sint32 draw_279_vertices[435];
static NJS_CNK_MODEL draw_279_model;
static NJS_CNK_OBJECT draw_279;
static Sint16 draw_266_polygons[501];
static Sint32 draw_266_vertices[431];
static NJS_CNK_MODEL draw_266_model;
static NJS_CNK_OBJECT draw_278;
static Sint16 draw_275_polygons[43];
static Sint32 draw_275_vertices[31];
static NJS_CNK_MODEL draw_275_model;
static NJS_CNK_OBJECT draw_275;
static Sint16 draw_274_polygons[39];
static Sint32 draw_274_vertices[27];
static NJS_CNK_MODEL draw_274_model;
static NJS_CNK_OBJECT draw_274;
static Sint16 draw_271_polygons[133];
static Sint32 draw_271_vertices[123];
static NJS_CNK_MODEL draw_271_model;
static NJS_CNK_OBJECT draw_271;
static Sint16 draw_270_polygons[241];
static Sint32 draw_270_vertices[135];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_270;
static Sint16 draw_269_polygons[401];
static Sint32 draw_269_vertices[291];
static NJS_CNK_MODEL draw_269_model;
static NJS_CNK_OBJECT draw_269;
static Sint16 draw_268_polygons[101];
static Sint32 draw_268_vertices[99];
static NJS_CNK_MODEL draw_268_model;
static NJS_CNK_OBJECT draw_268;
static Sint16 draw_267_polygons[101];
static Sint32 draw_267_vertices[83];
static NJS_CNK_MODEL draw_267_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_232_polygons[229];
static Sint32 draw_232_vertices[155];
static NJS_CNK_MODEL draw_232_model;
static NJS_CNK_OBJECT draw_264;
static Sint16 draw_263_polygons[263];
static Sint32 draw_263_vertices[219];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_263;
static Sint16 draw_251_polygons[89];
static Sint32 draw_251_vertices[51];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_262;
static Sint16 draw_252_polygons[289];
static Sint32 draw_252_vertices[179];
static NJS_CNK_MODEL draw_252_model;
static NJS_CNK_OBJECT draw_261;
static Sint16 draw_260_polygons[23];
static Sint32 draw_260_vertices[19];
static NJS_CNK_MODEL draw_260_model;
static NJS_CNK_OBJECT draw_260;
static Sint16 draw_259_polygons[161];
static Sint32 draw_259_vertices[107];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_259;
static Sint16 draw_258_polygons[115];
static Sint32 draw_258_vertices[63];
static NJS_CNK_MODEL draw_258_model;
static NJS_CNK_OBJECT draw_258;
static Sint16 draw_257_polygons[133];
static Sint32 draw_257_vertices[63];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_257;
static Sint16 draw_256_polygons[635];
static Sint32 draw_256_vertices[415];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_255_polygons[213];
static Sint32 draw_255_vertices[131];
static NJS_CNK_MODEL draw_255_model;
static NJS_CNK_OBJECT draw_255;
static Sint16 draw_254_polygons[209];
static Sint32 draw_254_vertices[123];
static NJS_CNK_MODEL draw_254_model;
static NJS_CNK_OBJECT draw_254;
static Sint16 draw_253_polygons[237];
static Sint32 draw_253_vertices[131];
static NJS_CNK_MODEL draw_253_model;
static NJS_CNK_OBJECT draw_253;
static Sint16 draw_250_polygons[153];
static Sint32 draw_250_vertices[159];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_249_polygons[261];
static Sint32 draw_249_vertices[283];
static NJS_CNK_MODEL draw_249_model;
static NJS_CNK_OBJECT draw_249;
static Sint16 draw_248_polygons[223];
static Sint32 draw_248_vertices[247];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_247_polygons[231];
static Sint32 draw_247_vertices[267];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_246_polygons[1103];
static Sint32 draw_246_vertices[935];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_245_polygons[63];
static Sint32 draw_245_vertices[67];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_45_polygons[223];
static Sint32 draw_45_vertices[163];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_244;
static Sint16 draw_60_polygons[243];
static Sint32 draw_60_vertices[163];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_243;
static Sint16 draw_242_polygons[63];
static Sint32 draw_242_vertices[63];
static NJS_CNK_MODEL draw_242_model;
static NJS_CNK_OBJECT draw_242;
static Sint16 draw_241_polygons[145];
static Sint32 draw_241_vertices[155];
static NJS_CNK_MODEL draw_241_model;
static NJS_CNK_OBJECT draw_241;
static Sint16 draw_240_polygons[151];
static Sint32 draw_240_vertices[107];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_240;
static Sint16 draw_239_polygons[23];
static Sint32 draw_239_vertices[19];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_59_polygons[219];
static Sint32 draw_59_vertices[199];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_237_polygons[195];
static Sint32 draw_237_vertices[223];
static NJS_CNK_MODEL draw_237_model;
static NJS_CNK_OBJECT draw_237;
static Sint16 draw_234_polygons[833];
static Sint32 draw_234_vertices[715];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_236;
static Sint16 draw_235_polygons[579];
static Sint32 draw_235_vertices[443];
static NJS_CNK_MODEL draw_235_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_172_polygons[273];
static Sint32 draw_172_vertices[207];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_155_polygons[579];
static Sint32 draw_155_vertices[555];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_158_polygons[3201];
static Sint32 draw_158_vertices[2951];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_230;
static Sint16 draw_229_polygons[263];
static Sint32 draw_229_vertices[191];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_228_polygons[359];
static Sint32 draw_228_vertices[243];
static NJS_CNK_MODEL draw_228_model;
static NJS_CNK_OBJECT draw_228;
static Sint16 draw_227_polygons[87];
static Sint32 draw_227_vertices[99];
static NJS_CNK_MODEL draw_227_model;
static NJS_CNK_OBJECT draw_227;
static Sint16 draw_226_polygons[201];
static Sint32 draw_226_vertices[167];
static NJS_CNK_MODEL draw_226_model;
static NJS_CNK_OBJECT draw_226;
static Sint16 draw_225_polygons[187];
static Sint32 draw_225_vertices[187];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_154_polygons[887];
static Sint32 draw_154_vertices[795];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_223;
static Sint16 draw_222_polygons[93];
static Sint32 draw_222_vertices[103];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_221_polygons[59];
static Sint32 draw_221_vertices[43];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_221;
static Sint16 draw_219_polygons[1463];
static Sint32 draw_219_vertices[1187];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_218_polygons[29];
static Sint32 draw_218_vertices[27];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_217_polygons[309];
static Sint32 draw_217_vertices[231];
static NJS_CNK_MODEL draw_217_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_216_polygons[75];
static Sint32 draw_216_vertices[75];
static NJS_CNK_MODEL draw_216_model;
static NJS_CNK_OBJECT draw_216;
static Sint16 draw_215_polygons[217];
static Sint32 draw_215_vertices[219];
static NJS_CNK_MODEL draw_215_model;
static NJS_CNK_OBJECT draw_215;
static Sint16 draw_214_polygons[121];
static Sint32 draw_214_vertices[115];
static NJS_CNK_MODEL draw_214_model;
static NJS_CNK_OBJECT draw_214;
static Sint16 draw_213_polygons[205];
static Sint32 draw_213_vertices[243];
static NJS_CNK_MODEL draw_213_model;
static NJS_CNK_OBJECT draw_213;
static Sint16 draw_212_polygons[115];
static Sint32 draw_212_vertices[107];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_212;
static Sint16 draw_211_polygons[127];
static Sint32 draw_211_vertices[111];
static NJS_CNK_MODEL draw_211_model;
static NJS_CNK_OBJECT draw_211;
static Sint16 draw_210_polygons[61];
static Sint32 draw_210_vertices[63];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_210;
static Sint16 draw_209_polygons[113];
static Sint32 draw_209_vertices[131];
static NJS_CNK_MODEL draw_209_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_208_polygons[29];
static Sint32 draw_208_vertices[27];
static NJS_CNK_MODEL draw_208_model;
static NJS_CNK_OBJECT draw_208;
static Sint16 draw_207_polygons[115];
static Sint32 draw_207_vertices[123];
static NJS_CNK_MODEL draw_207_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_206_polygons[217];
static Sint32 draw_206_vertices[239];
static NJS_CNK_MODEL draw_206_model;
static NJS_CNK_OBJECT draw_206;
static Sint16 draw_205_polygons[417];
static Sint32 draw_205_vertices[327];
static NJS_CNK_MODEL draw_205_model;
static NJS_CNK_OBJECT draw_205;
static Sint16 draw_199_polygons[207];
static Sint32 draw_199_vertices[219];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_200;
static Sint16 draw_198_polygons[339];
static Sint32 draw_198_vertices[355];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_195_polygons[75];
static Sint32 draw_195_vertices[67];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_194_polygons[37];
static Sint32 draw_194_vertices[31];
static NJS_CNK_MODEL draw_194_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_193_polygons[929];
static Sint32 draw_193_vertices[771];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_192_polygons[1011];
static Sint32 draw_192_vertices[811];
static NJS_CNK_MODEL draw_192_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_191_polygons[611];
static Sint32 draw_191_vertices[655];
static NJS_CNK_MODEL draw_191_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_190_polygons[279];
static Sint32 draw_190_vertices[231];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_190;
static Sint16 draw_189_polygons[229];
static Sint32 draw_189_vertices[207];
static NJS_CNK_MODEL draw_189_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_186_polygons[107];
static Sint32 draw_186_vertices[119];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_186;
static Sint16 draw_185_polygons[501];
static Sint32 draw_185_vertices[439];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_184_polygons[113];
static Sint32 draw_184_vertices[107];
static NJS_CNK_MODEL draw_184_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_183_polygons[49];
static Sint32 draw_183_vertices[47];
static NJS_CNK_MODEL draw_183_model;
static NJS_CNK_OBJECT draw_183;
static Sint16 draw_180_polygons[935];
static Sint32 draw_180_vertices[683];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_178_polygons[63];
static Sint32 draw_178_vertices[59];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_176_polygons[69];
static Sint32 draw_176_vertices[27];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_170_polygons[595];
static Sint32 draw_170_vertices[515];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_170;
static Sint16 draw_169_polygons[189];
static Sint32 draw_169_vertices[171];
static NJS_CNK_MODEL draw_169_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_168_polygons[1223];
static Sint32 draw_168_vertices[963];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_17_polygons[461];
static Sint32 draw_17_vertices[343];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_160;
static Sint16 draw_35_polygons[509];
static Sint32 draw_35_vertices[451];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_147_polygons[313];
static Sint32 draw_147_vertices[219];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_146_polygons[239];
static Sint32 draw_146_vertices[167];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_145_polygons[35];
static Sint32 draw_145_vertices[35];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_144_polygons[35];
static Sint32 draw_144_vertices[35];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_143_polygons[67];
static Sint32 draw_143_vertices[43];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_142_polygons[37];
static Sint32 draw_142_vertices[31];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_141_polygons[1125];
static Sint32 draw_141_vertices[863];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_140_polygons[193];
static Sint32 draw_140_vertices[175];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_139_polygons[201];
static Sint32 draw_139_vertices[187];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_138_polygons[131];
static Sint32 draw_138_vertices[111];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[91];
static Sint32 draw_137_vertices[79];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[239];
static Sint32 draw_136_vertices[143];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[99];
static Sint32 draw_135_vertices[75];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_134_polygons[445];
static Sint32 draw_134_vertices[359];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[631];
static Sint32 draw_133_vertices[447];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[91];
static Sint32 draw_132_vertices[55];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_43_polygons[63];
static Sint32 draw_43_vertices[55];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_39_polygons[133];
static Sint32 draw_39_vertices[123];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[209];
static Sint32 draw_123_vertices[179];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_121_polygons[331];
static Sint32 draw_121_vertices[283];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[1991];
static Sint32 draw_120_vertices[1743];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_113_polygons[393];
static Sint32 draw_113_vertices[395];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[69];
static Sint32 draw_112_vertices[27];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_109_polygons[63];
static Sint32 draw_109_vertices[55];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_108_polygons[349];
static Sint32 draw_108_vertices[263];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_107_polygons[1913];
static Sint32 draw_107_vertices[1731];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_3_polygons[601];
static Sint32 draw_3_vertices[447];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[417];
static Sint32 draw_105_vertices[311];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[297];
static Sint32 draw_104_vertices[183];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[265];
static Sint32 draw_103_vertices[191];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_29_polygons[565];
static Sint32 draw_29_vertices[399];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[607];
static Sint32 draw_101_vertices[475];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_99_polygons[245];
static Sint32 draw_99_vertices[263];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[49];
static Sint32 draw_98_vertices[43];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_97_polygons[461];
static Sint32 draw_97_vertices[387];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[163];
static Sint32 draw_96_vertices[103];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_81_polygons[695];
static Sint32 draw_81_vertices[599];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[127];
static Sint32 draw_92_vertices[111];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_32_polygons[3731];
static Sint32 draw_32_vertices[3215];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_14_polygons[69];
static Sint32 draw_14_vertices[51];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_80_polygons[259];
static Sint32 draw_80_vertices[207];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_12_polygons[139];
static Sint32 draw_12_vertices[123];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_77_polygons[69];
static Sint32 draw_77_vertices[67];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_74_polygons[89];
static Sint32 draw_74_vertices[91];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_75_polygons[49];
static Sint32 draw_75_vertices[43];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_73_polygons[61];
static Sint32 draw_73_vertices[63];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[693];
static Sint32 draw_72_vertices[563];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[2321];
static Sint32 draw_71_vertices[2087];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[1653];
static Sint32 draw_70_vertices[1479];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[207];
static Sint32 draw_69_vertices[167];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[1237];
static Sint32 draw_68_vertices[1015];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[439];
static Sint32 draw_67_vertices[259];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[271];
static Sint32 draw_65_vertices[223];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_64_polygons[117];
static Sint32 draw_64_vertices[91];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[239];
static Sint32 draw_63_vertices[223];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_58_polygons[153];
static Sint32 draw_58_vertices[171];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[23];
static Sint32 draw_57_vertices[19];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[23];
static Sint32 draw_56_vertices[19];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[23];
static Sint32 draw_55_vertices[19];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[23];
static Sint32 draw_54_vertices[19];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_1_polygons[455];
static Sint32 draw_1_vertices[399];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_0_polygons[547];
static Sint32 draw_0_vertices[463];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[401];
static Sint32 draw_51_vertices[247];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_47_polygons[357];
static Sint32 draw_47_vertices[339];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_34_polygons[495];
static Sint32 draw_34_vertices[415];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_31_polygons[1779];
static Sint32 draw_31_vertices[1407];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[177];
static Sint32 draw_30_vertices[155];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_25_polygons[383];
static Sint32 draw_25_vertices[295];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_22_polygons[1825];
static Sint32 draw_22_vertices[1651];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_13_polygons[311];
static Sint32 draw_13_vertices[235];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_11_polygons[101];
static Sint32 draw_11_vertices[103];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[229];
static Sint32 draw_10_vertices[219];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[67];
static Sint32 draw_9_vertices[47];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_7_polygons[265];
static Sint32 draw_7_vertices[191];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[189];
static Sint32 draw_6_vertices[115];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_4_polygons[13];
static Sint32 draw_4_vertices[19];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_119;
static NJS_OBJECT collision_120;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_144;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_156;
static NJS_OBJECT collision_157;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_159;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_177;
static NJS_OBJECT collision_178;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_181;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_283;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_277;
static NJS_CNK_OBJECT draw_276;
static NJS_CNK_OBJECT draw_273;
static NJS_CNK_OBJECT draw_272;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_251;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_203;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_167;
static NJS_CNK_OBJECT draw_166;
static NJS_CNK_OBJECT draw_165;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_161;
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
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
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
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
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
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[480] = {
#include "stg03/terrain/entries.inc"
};
struct land_table green_forest_land = {
#include "stg03/terrain/green_forest_land.inc"
};
static NJS_TEXNAME textures[38] = {
#include "stg03/terrain/textures.inc"
};
NJS_TEXLIST green_forest_ground_texlist = {
#include "stg03/terrain/green_forest_ground_texlist.inc"
};
NJS_TEXLIST green_forest_water_texlist = {
#include "stg03/terrain/green_forest_water_texlist.inc"
};
NJS_TEXLIST green_forest_land_texlist = {
#include "stg03/terrain/green_forest_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg03/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[53] = {
#include "stg03/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg03/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[28] = {
#include "stg03/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg03/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg03/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg03/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[53] = {
#include "stg03/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg03/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[27] = {
#include "stg03/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg03/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg03/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg03/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[135] = {
#include "stg03/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg03/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[78] = {
#include "stg03/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg03/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg03/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg03/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[24] = {
#include "stg03/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg03/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[16] = {
#include "stg03/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg03/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg03/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg03/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[25] = {
#include "stg03/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg03/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[14] = {
#include "stg03/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg03/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg03/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg03/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[37] = {
#include "stg03/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg03/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[24] = {
#include "stg03/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg03/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg03/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg03/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[61] = {
#include "stg03/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg03/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[29] = {
#include "stg03/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg03/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg03/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg03/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[70] = {
#include "stg03/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg03/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[37] = {
#include "stg03/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg03/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg03/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg03/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[63] = {
#include "stg03/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg03/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[36] = {
#include "stg03/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg03/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg03/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg03/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[29] = {
#include "stg03/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg03/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[16] = {
#include "stg03/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg03/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg03/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg03/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[95] = {
#include "stg03/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg03/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[48] = {
#include "stg03/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg03/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg03/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg03/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[54] = {
#include "stg03/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg03/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[29] = {
#include "stg03/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg03/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg03/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg03/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[118] = {
#include "stg03/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg03/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[55] = {
#include "stg03/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg03/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg03/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg03/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[51] = {
#include "stg03/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg03/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[32] = {
#include "stg03/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg03/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg03/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg03/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[28] = {
#include "stg03/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg03/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[15] = {
#include "stg03/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg03/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg03/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg03/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[132] = {
#include "stg03/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg03/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[68] = {
#include "stg03/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg03/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg03/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg03/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[31] = {
#include "stg03/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg03/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[16] = {
#include "stg03/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg03/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg03/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg03/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[9] = {
#include "stg03/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg03/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[8] = {
#include "stg03/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg03/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg03/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg03/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[12] = {
#include "stg03/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg03/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[8] = {
#include "stg03/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg03/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg03/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg03/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[12] = {
#include "stg03/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg03/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg03/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg03/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg03/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg03/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[54] = {
#include "stg03/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg03/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[39] = {
#include "stg03/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg03/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg03/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg03/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[55] = {
#include "stg03/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg03/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[30] = {
#include "stg03/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg03/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg03/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg03/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[65] = {
#include "stg03/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg03/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[36] = {
#include "stg03/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg03/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg03/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg03/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[26] = {
#include "stg03/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg03/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[18] = {
#include "stg03/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg03/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg03/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg03/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[36] = {
#include "stg03/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg03/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[24] = {
#include "stg03/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg03/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg03/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg03/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[36] = {
#include "stg03/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg03/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[22] = {
#include "stg03/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg03/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg03/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg03/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[19] = {
#include "stg03/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg03/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[18] = {
#include "stg03/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg03/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg03/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg03/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[14] = {
#include "stg03/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg03/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[9] = {
#include "stg03/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg03/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg03/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg03/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[60] = {
#include "stg03/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg03/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[32] = {
#include "stg03/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg03/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg03/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg03/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[37] = {
#include "stg03/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg03/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[22] = {
#include "stg03/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg03/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg03/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg03/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[150] = {
#include "stg03/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg03/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[84] = {
#include "stg03/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg03/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg03/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg03/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[28] = {
#include "stg03/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg03/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[15] = {
#include "stg03/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg03/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg03/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg03/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[111] = {
#include "stg03/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg03/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[50] = {
#include "stg03/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg03/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg03/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg03/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[104] = {
#include "stg03/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg03/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[54] = {
#include "stg03/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg03/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg03/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg03/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[24] = {
#include "stg03/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg03/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[16] = {
#include "stg03/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg03/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg03/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg03/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[47] = {
#include "stg03/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg03/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[23] = {
#include "stg03/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg03/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg03/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg03/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[52] = {
#include "stg03/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg03/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[22] = {
#include "stg03/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg03/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg03/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg03/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[5] = {
#include "stg03/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg03/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[4] = {
#include "stg03/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg03/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg03/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg03/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[51] = {
#include "stg03/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg03/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[26] = {
#include "stg03/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg03/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg03/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg03/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[105] = {
#include "stg03/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg03/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[60] = {
#include "stg03/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg03/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg03/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg03/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[29] = {
#include "stg03/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg03/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[18] = {
#include "stg03/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg03/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg03/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg03/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[77] = {
#include "stg03/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg03/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[44] = {
#include "stg03/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg03/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg03/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg03/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[272] = {
#include "stg03/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg03/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[100] = {
#include "stg03/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg03/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg03/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg03/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[49] = {
#include "stg03/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg03/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[22] = {
#include "stg03/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg03/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg03/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg03/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[32] = {
#include "stg03/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg03/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[17] = {
#include "stg03/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg03/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg03/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg03/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[567] = {
#include "stg03/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg03/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[310] = {
#include "stg03/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg03/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg03/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg03/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[78] = {
#include "stg03/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg03/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[36] = {
#include "stg03/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg03/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg03/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg03/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[77] = {
#include "stg03/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg03/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[30] = {
#include "stg03/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg03/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg03/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg03/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[135] = {
#include "stg03/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg03/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[53] = {
#include "stg03/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg03/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg03/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg03/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[43] = {
#include "stg03/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg03/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[22] = {
#include "stg03/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg03/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg03/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg03/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[567] = {
#include "stg03/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg03/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[310] = {
#include "stg03/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg03/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg03/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg03/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[103] = {
#include "stg03/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg03/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[58] = {
#include "stg03/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg03/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg03/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg03/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[50] = {
#include "stg03/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg03/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[26] = {
#include "stg03/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg03/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg03/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg03/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[153] = {
#include "stg03/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg03/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[74] = {
#include "stg03/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg03/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg03/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg03/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[72] = {
#include "stg03/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg03/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[32] = {
#include "stg03/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg03/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg03/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg03/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[45] = {
#include "stg03/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg03/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[24] = {
#include "stg03/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg03/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg03/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg03/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[92] = {
#include "stg03/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg03/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[39] = {
#include "stg03/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg03/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg03/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg03/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[124] = {
#include "stg03/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg03/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[75] = {
#include "stg03/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg03/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg03/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg03/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[31] = {
#include "stg03/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg03/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[16] = {
#include "stg03/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg03/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg03/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg03/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[42] = {
#include "stg03/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg03/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[30] = {
#include "stg03/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg03/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg03/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg03/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[53] = {
#include "stg03/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg03/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[28] = {
#include "stg03/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg03/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg03/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg03/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[96] = {
#include "stg03/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg03/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[43] = {
#include "stg03/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg03/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg03/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg03/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[91] = {
#include "stg03/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg03/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[48] = {
#include "stg03/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg03/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg03/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg03/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[56] = {
#include "stg03/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg03/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[25] = {
#include "stg03/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg03/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg03/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg03/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[103] = {
#include "stg03/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg03/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[58] = {
#include "stg03/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg03/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg03/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg03/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[82] = {
#include "stg03/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg03/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[41] = {
#include "stg03/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg03/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg03/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg03/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[51] = {
#include "stg03/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg03/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[29] = {
#include "stg03/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg03/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg03/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg03/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[64] = {
#include "stg03/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg03/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[29] = {
#include "stg03/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg03/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg03/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg03/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[79] = {
#include "stg03/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg03/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[38] = {
#include "stg03/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg03/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg03/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg03/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[79] = {
#include "stg03/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg03/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[47] = {
#include "stg03/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg03/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg03/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg03/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[190] = {
#include "stg03/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg03/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[81] = {
#include "stg03/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg03/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg03/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg03/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[190] = {
#include "stg03/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg03/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[89] = {
#include "stg03/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg03/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg03/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg03/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[155] = {
#include "stg03/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg03/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[75] = {
#include "stg03/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg03/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg03/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg03/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[49] = {
#include "stg03/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg03/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[20] = {
#include "stg03/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg03/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg03/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg03/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[50] = {
#include "stg03/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg03/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[25] = {
#include "stg03/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg03/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg03/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg03/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[97] = {
#include "stg03/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg03/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[42] = {
#include "stg03/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg03/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg03/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg03/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[43] = {
#include "stg03/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg03/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[27] = {
#include "stg03/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg03/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg03/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg03/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[297] = {
#include "stg03/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg03/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[160] = {
#include "stg03/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg03/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg03/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg03/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[15] = {
#include "stg03/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg03/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[11] = {
#include "stg03/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg03/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg03/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg03/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[18] = {
#include "stg03/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg03/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[11] = {
#include "stg03/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg03/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg03/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg03/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[189] = {
#include "stg03/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg03/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[91] = {
#include "stg03/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg03/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg03/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg03/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[11] = {
#include "stg03/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg03/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[10] = {
#include "stg03/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg03/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg03/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg03/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[13] = {
#include "stg03/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg03/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[12] = {
#include "stg03/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg03/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg03/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg03/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[30] = {
#include "stg03/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg03/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[18] = {
#include "stg03/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg03/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg03/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg03/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[51] = {
#include "stg03/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg03/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[24] = {
#include "stg03/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg03/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg03/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg03/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[9] = {
#include "stg03/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg03/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[8] = {
#include "stg03/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg03/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg03/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg03/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[35] = {
#include "stg03/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg03/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[22] = {
#include "stg03/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg03/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg03/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg03/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[172] = {
#include "stg03/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg03/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[68] = {
#include "stg03/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg03/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg03/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg03/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[245] = {
#include "stg03/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg03/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[97] = {
#include "stg03/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg03/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg03/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg03/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[617] = {
#include "stg03/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg03/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[338] = {
#include "stg03/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg03/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg03/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg03/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[114] = {
#include "stg03/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg03/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[76] = {
#include "stg03/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg03/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg03/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg03/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[26] = {
#include "stg03/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg03/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[18] = {
#include "stg03/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg03/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg03/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg03/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[45] = {
#include "stg03/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg03/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[24] = {
#include "stg03/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg03/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg03/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg03/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[22] = {
#include "stg03/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg03/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[15] = {
#include "stg03/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg03/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg03/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_109_materials[1] = {
#include "stg03/terrain/collision_109_materials.inc"
};
static Sint16 collision_109_faces[26] = {
#include "stg03/terrain/collision_109_faces.inc"
};
static NJS_MESHSET collision_109_meshsets[1] = {
#include "stg03/terrain/collision_109_meshsets.inc"
};
static NJS_POINT3 collision_109_points[18] = {
#include "stg03/terrain/collision_109_points.inc"
};
static NJS_MODEL collision_109_model = {
#include "stg03/terrain/collision_109_model.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg03/terrain/collision_109.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg03/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[45] = {
#include "stg03/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg03/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[24] = {
#include "stg03/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg03/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg03/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg03/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[43] = {
#include "stg03/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg03/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[23] = {
#include "stg03/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg03/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg03/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg03/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[26] = {
#include "stg03/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg03/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[18] = {
#include "stg03/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg03/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg03/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg03/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[18] = {
#include "stg03/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg03/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[12] = {
#include "stg03/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg03/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg03/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg03/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[18] = {
#include "stg03/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg03/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[12] = {
#include "stg03/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg03/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg03/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg03/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[27] = {
#include "stg03/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg03/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[17] = {
#include "stg03/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg03/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg03/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg03/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[30] = {
#include "stg03/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg03/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[18] = {
#include "stg03/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg03/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg03/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg03/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[68] = {
#include "stg03/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg03/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[32] = {
#include "stg03/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg03/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg03/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg03/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[222] = {
#include "stg03/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg03/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[99] = {
#include "stg03/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg03/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg03/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg03/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[25] = {
#include "stg03/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg03/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[18] = {
#include "stg03/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg03/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg03/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg03/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[22] = {
#include "stg03/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg03/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[15] = {
#include "stg03/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg03/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg03/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg03/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[65] = {
#include "stg03/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg03/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[30] = {
#include "stg03/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg03/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg03/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg03/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[205] = {
#include "stg03/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg03/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[87] = {
#include "stg03/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg03/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg03/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_126_materials[1] = {
#include "stg03/terrain/collision_126_materials.inc"
};
static Sint16 collision_126_faces[55] = {
#include "stg03/terrain/collision_126_faces.inc"
};
static NJS_MESHSET collision_126_meshsets[1] = {
#include "stg03/terrain/collision_126_meshsets.inc"
};
static NJS_POINT3 collision_126_points[40] = {
#include "stg03/terrain/collision_126_points.inc"
};
static NJS_MODEL collision_126_model = {
#include "stg03/terrain/collision_126_model.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg03/terrain/collision_126.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg03/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[136] = {
#include "stg03/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg03/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[54] = {
#include "stg03/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg03/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg03/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_128_materials[1] = {
#include "stg03/terrain/collision_128_materials.inc"
};
static Sint16 collision_128_faces[67] = {
#include "stg03/terrain/collision_128_faces.inc"
};
static NJS_MESHSET collision_128_meshsets[1] = {
#include "stg03/terrain/collision_128_meshsets.inc"
};
static NJS_POINT3 collision_128_points[30] = {
#include "stg03/terrain/collision_128_points.inc"
};
static NJS_MODEL collision_128_model = {
#include "stg03/terrain/collision_128_model.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg03/terrain/collision_128.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg03/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[25] = {
#include "stg03/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg03/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[14] = {
#include "stg03/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg03/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg03/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg03/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[9] = {
#include "stg03/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg03/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[8] = {
#include "stg03/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg03/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg03/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg03/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[51] = {
#include "stg03/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg03/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[21] = {
#include "stg03/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg03/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg03/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_132_materials[1] = {
#include "stg03/terrain/collision_132_materials.inc"
};
static Sint16 collision_132_faces[45] = {
#include "stg03/terrain/collision_132_faces.inc"
};
static NJS_MESHSET collision_132_meshsets[1] = {
#include "stg03/terrain/collision_132_meshsets.inc"
};
static NJS_POINT3 collision_132_points[23] = {
#include "stg03/terrain/collision_132_points.inc"
};
static NJS_MODEL collision_132_model = {
#include "stg03/terrain/collision_132_model.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg03/terrain/collision_132.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg03/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[20] = {
#include "stg03/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg03/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[9] = {
#include "stg03/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg03/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg03/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg03/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[26] = {
#include "stg03/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg03/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[11] = {
#include "stg03/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg03/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg03/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg03/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[69] = {
#include "stg03/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg03/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[34] = {
#include "stg03/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg03/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg03/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg03/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[15] = {
#include "stg03/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg03/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[7] = {
#include "stg03/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg03/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg03/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg03/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[83] = {
#include "stg03/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg03/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[44] = {
#include "stg03/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg03/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg03/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg03/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[186] = {
#include "stg03/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg03/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[72] = {
#include "stg03/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg03/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg03/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg03/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[63] = {
#include "stg03/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg03/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[40] = {
#include "stg03/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg03/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg03/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg03/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[25] = {
#include "stg03/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg03/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[18] = {
#include "stg03/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg03/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg03/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg03/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[9] = {
#include "stg03/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg03/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[8] = {
#include "stg03/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg03/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg03/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg03/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[21] = {
#include "stg03/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg03/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[20] = {
#include "stg03/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg03/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg03/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg03/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[21] = {
#include "stg03/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg03/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[20] = {
#include "stg03/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg03/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg03/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg03/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[111] = {
#include "stg03/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg03/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[60] = {
#include "stg03/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg03/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg03/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg03/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[112] = {
#include "stg03/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg03/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[50] = {
#include "stg03/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg03/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg03/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg03/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[117] = {
#include "stg03/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg03/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[60] = {
#include "stg03/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg03/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg03/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg03/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[341] = {
#include "stg03/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg03/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[184] = {
#include "stg03/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg03/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg03/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg03/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[366] = {
#include "stg03/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg03/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[183] = {
#include "stg03/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg03/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg03/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg03/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[119] = {
#include "stg03/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg03/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[53] = {
#include "stg03/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg03/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg03/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg03/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[399] = {
#include "stg03/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg03/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[210] = {
#include "stg03/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg03/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg03/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg03/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[115] = {
#include "stg03/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg03/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[62] = {
#include "stg03/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg03/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg03/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg03/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[323] = {
#include "stg03/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg03/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[169] = {
#include "stg03/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg03/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg03/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg03/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[64] = {
#include "stg03/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg03/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[26] = {
#include "stg03/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg03/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg03/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg03/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[387] = {
#include "stg03/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg03/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[210] = {
#include "stg03/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg03/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg03/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg03/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[84] = {
#include "stg03/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg03/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[50] = {
#include "stg03/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg03/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg03/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg03/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[43] = {
#include "stg03/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg03/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[26] = {
#include "stg03/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg03/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg03/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg03/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[63] = {
#include "stg03/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg03/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[40] = {
#include "stg03/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg03/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg03/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg03/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[5] = {
#include "stg03/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg03/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[4] = {
#include "stg03/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg03/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg03/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg03/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[7] = {
#include "stg03/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg03/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[6] = {
#include "stg03/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg03/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg03/terrain/collision_185.inc"
};
static Sint16 draw_40_polygons[221] = {
#include "stg03/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[207] = {
#include "stg03/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg03/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg03/terrain/draw_293.inc"
};
static Sint16 draw_15_polygons[69] = {
#include "stg03/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[51] = {
#include "stg03/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg03/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg03/terrain/draw_292.inc"
};
static Sint16 draw_16_polygons[235] = {
#include "stg03/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[215] = {
#include "stg03/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg03/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg03/terrain/draw_290.inc"
};
static Sint16 draw_289_polygons[133] = {
#include "stg03/terrain/draw_289_polygons.inc"
};
static Sint32 draw_289_vertices[123] = {
#include "stg03/terrain/draw_289_vertices.inc"
};
static NJS_CNK_MODEL draw_289_model = {
#include "stg03/terrain/draw_289_model.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg03/terrain/draw_289.inc"
};
static Sint16 draw_288_polygons[139] = {
#include "stg03/terrain/draw_288_polygons.inc"
};
static Sint32 draw_288_vertices[111] = {
#include "stg03/terrain/draw_288_vertices.inc"
};
static NJS_CNK_MODEL draw_288_model = {
#include "stg03/terrain/draw_288_model.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg03/terrain/draw_288.inc"
};
static Sint16 draw_37_polygons[499] = {
#include "stg03/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[387] = {
#include "stg03/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg03/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg03/terrain/draw_287.inc"
};
static Sint16 draw_33_polygons[499] = {
#include "stg03/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[395] = {
#include "stg03/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg03/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg03/terrain/draw_286.inc"
};
static Sint16 draw_285_polygons[633] = {
#include "stg03/terrain/draw_285_polygons.inc"
};
static Sint32 draw_285_vertices[507] = {
#include "stg03/terrain/draw_285_vertices.inc"
};
static NJS_CNK_MODEL draw_285_model = {
#include "stg03/terrain/draw_285_model.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg03/terrain/draw_285.inc"
};
static Sint16 draw_21_polygons[129] = {
#include "stg03/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[99] = {
#include "stg03/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg03/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg03/terrain/draw_284.inc"
};
static Sint16 draw_19_polygons[961] = {
#include "stg03/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[999] = {
#include "stg03/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg03/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg03/terrain/draw_282.inc"
};
static Sint16 draw_280_polygons[43] = {
#include "stg03/terrain/draw_280_polygons.inc"
};
static Sint32 draw_280_vertices[31] = {
#include "stg03/terrain/draw_280_vertices.inc"
};
static NJS_CNK_MODEL draw_280_model = {
#include "stg03/terrain/draw_280_model.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg03/terrain/draw_280.inc"
};
static Sint16 draw_279_polygons[415] = {
#include "stg03/terrain/draw_279_polygons.inc"
};
static Sint32 draw_279_vertices[435] = {
#include "stg03/terrain/draw_279_vertices.inc"
};
static NJS_CNK_MODEL draw_279_model = {
#include "stg03/terrain/draw_279_model.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg03/terrain/draw_279.inc"
};
static Sint16 draw_266_polygons[501] = {
#include "stg03/terrain/draw_266_polygons.inc"
};
static Sint32 draw_266_vertices[431] = {
#include "stg03/terrain/draw_266_vertices.inc"
};
static NJS_CNK_MODEL draw_266_model = {
#include "stg03/terrain/draw_266_model.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg03/terrain/draw_278.inc"
};
static Sint16 draw_275_polygons[43] = {
#include "stg03/terrain/draw_275_polygons.inc"
};
static Sint32 draw_275_vertices[31] = {
#include "stg03/terrain/draw_275_vertices.inc"
};
static NJS_CNK_MODEL draw_275_model = {
#include "stg03/terrain/draw_275_model.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg03/terrain/draw_275.inc"
};
static Sint16 draw_274_polygons[39] = {
#include "stg03/terrain/draw_274_polygons.inc"
};
static Sint32 draw_274_vertices[27] = {
#include "stg03/terrain/draw_274_vertices.inc"
};
static NJS_CNK_MODEL draw_274_model = {
#include "stg03/terrain/draw_274_model.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg03/terrain/draw_274.inc"
};
static Sint16 draw_271_polygons[133] = {
#include "stg03/terrain/draw_271_polygons.inc"
};
static Sint32 draw_271_vertices[123] = {
#include "stg03/terrain/draw_271_vertices.inc"
};
static NJS_CNK_MODEL draw_271_model = {
#include "stg03/terrain/draw_271_model.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg03/terrain/draw_271.inc"
};
static Sint16 draw_270_polygons[241] = {
#include "stg03/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[135] = {
#include "stg03/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg03/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg03/terrain/draw_270.inc"
};
static Sint16 draw_269_polygons[401] = {
#include "stg03/terrain/draw_269_polygons.inc"
};
static Sint32 draw_269_vertices[291] = {
#include "stg03/terrain/draw_269_vertices.inc"
};
static NJS_CNK_MODEL draw_269_model = {
#include "stg03/terrain/draw_269_model.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg03/terrain/draw_269.inc"
};
static Sint16 draw_268_polygons[101] = {
#include "stg03/terrain/draw_268_polygons.inc"
};
static Sint32 draw_268_vertices[99] = {
#include "stg03/terrain/draw_268_vertices.inc"
};
static NJS_CNK_MODEL draw_268_model = {
#include "stg03/terrain/draw_268_model.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg03/terrain/draw_268.inc"
};
static Sint16 draw_267_polygons[101] = {
#include "stg03/terrain/draw_267_polygons.inc"
};
static Sint32 draw_267_vertices[83] = {
#include "stg03/terrain/draw_267_vertices.inc"
};
static NJS_CNK_MODEL draw_267_model = {
#include "stg03/terrain/draw_267_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg03/terrain/draw_267.inc"
};
static Sint16 draw_232_polygons[229] = {
#include "stg03/terrain/draw_232_polygons.inc"
};
static Sint32 draw_232_vertices[155] = {
#include "stg03/terrain/draw_232_vertices.inc"
};
static NJS_CNK_MODEL draw_232_model = {
#include "stg03/terrain/draw_232_model.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg03/terrain/draw_264.inc"
};
static Sint16 draw_263_polygons[263] = {
#include "stg03/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[219] = {
#include "stg03/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg03/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg03/terrain/draw_263.inc"
};
static Sint16 draw_251_polygons[89] = {
#include "stg03/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[51] = {
#include "stg03/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg03/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg03/terrain/draw_262.inc"
};
static Sint16 draw_252_polygons[289] = {
#include "stg03/terrain/draw_252_polygons.inc"
};
static Sint32 draw_252_vertices[179] = {
#include "stg03/terrain/draw_252_vertices.inc"
};
static NJS_CNK_MODEL draw_252_model = {
#include "stg03/terrain/draw_252_model.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg03/terrain/draw_261.inc"
};
static Sint16 draw_260_polygons[23] = {
#include "stg03/terrain/draw_260_polygons.inc"
};
static Sint32 draw_260_vertices[19] = {
#include "stg03/terrain/draw_260_vertices.inc"
};
static NJS_CNK_MODEL draw_260_model = {
#include "stg03/terrain/draw_260_model.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg03/terrain/draw_260.inc"
};
static Sint16 draw_259_polygons[161] = {
#include "stg03/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[107] = {
#include "stg03/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg03/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg03/terrain/draw_259.inc"
};
static Sint16 draw_258_polygons[115] = {
#include "stg03/terrain/draw_258_polygons.inc"
};
static Sint32 draw_258_vertices[63] = {
#include "stg03/terrain/draw_258_vertices.inc"
};
static NJS_CNK_MODEL draw_258_model = {
#include "stg03/terrain/draw_258_model.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg03/terrain/draw_258.inc"
};
static Sint16 draw_257_polygons[133] = {
#include "stg03/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[63] = {
#include "stg03/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg03/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg03/terrain/draw_257.inc"
};
static Sint16 draw_256_polygons[635] = {
#include "stg03/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[415] = {
#include "stg03/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg03/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg03/terrain/draw_256.inc"
};
static Sint16 draw_255_polygons[213] = {
#include "stg03/terrain/draw_255_polygons.inc"
};
static Sint32 draw_255_vertices[131] = {
#include "stg03/terrain/draw_255_vertices.inc"
};
static NJS_CNK_MODEL draw_255_model = {
#include "stg03/terrain/draw_255_model.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg03/terrain/draw_255.inc"
};
static Sint16 draw_254_polygons[209] = {
#include "stg03/terrain/draw_254_polygons.inc"
};
static Sint32 draw_254_vertices[123] = {
#include "stg03/terrain/draw_254_vertices.inc"
};
static NJS_CNK_MODEL draw_254_model = {
#include "stg03/terrain/draw_254_model.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg03/terrain/draw_254.inc"
};
static Sint16 draw_253_polygons[237] = {
#include "stg03/terrain/draw_253_polygons.inc"
};
static Sint32 draw_253_vertices[131] = {
#include "stg03/terrain/draw_253_vertices.inc"
};
static NJS_CNK_MODEL draw_253_model = {
#include "stg03/terrain/draw_253_model.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg03/terrain/draw_253.inc"
};
static Sint16 draw_250_polygons[153] = {
#include "stg03/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[159] = {
#include "stg03/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg03/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg03/terrain/draw_250.inc"
};
static Sint16 draw_249_polygons[261] = {
#include "stg03/terrain/draw_249_polygons.inc"
};
static Sint32 draw_249_vertices[283] = {
#include "stg03/terrain/draw_249_vertices.inc"
};
static NJS_CNK_MODEL draw_249_model = {
#include "stg03/terrain/draw_249_model.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg03/terrain/draw_249.inc"
};
static Sint16 draw_248_polygons[223] = {
#include "stg03/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[247] = {
#include "stg03/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg03/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg03/terrain/draw_248.inc"
};
static Sint16 draw_247_polygons[231] = {
#include "stg03/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[267] = {
#include "stg03/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg03/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg03/terrain/draw_247.inc"
};
static Sint16 draw_246_polygons[1103] = {
#include "stg03/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[935] = {
#include "stg03/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg03/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg03/terrain/draw_246.inc"
};
static Sint16 draw_245_polygons[63] = {
#include "stg03/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[67] = {
#include "stg03/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg03/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg03/terrain/draw_245.inc"
};
static Sint16 draw_45_polygons[223] = {
#include "stg03/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[163] = {
#include "stg03/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg03/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg03/terrain/draw_244.inc"
};
static Sint16 draw_60_polygons[243] = {
#include "stg03/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[163] = {
#include "stg03/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg03/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg03/terrain/draw_243.inc"
};
static Sint16 draw_242_polygons[63] = {
#include "stg03/terrain/draw_242_polygons.inc"
};
static Sint32 draw_242_vertices[63] = {
#include "stg03/terrain/draw_242_vertices.inc"
};
static NJS_CNK_MODEL draw_242_model = {
#include "stg03/terrain/draw_242_model.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg03/terrain/draw_242.inc"
};
static Sint16 draw_241_polygons[145] = {
#include "stg03/terrain/draw_241_polygons.inc"
};
static Sint32 draw_241_vertices[155] = {
#include "stg03/terrain/draw_241_vertices.inc"
};
static NJS_CNK_MODEL draw_241_model = {
#include "stg03/terrain/draw_241_model.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg03/terrain/draw_241.inc"
};
static Sint16 draw_240_polygons[151] = {
#include "stg03/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[107] = {
#include "stg03/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg03/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg03/terrain/draw_240.inc"
};
static Sint16 draw_239_polygons[23] = {
#include "stg03/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[19] = {
#include "stg03/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg03/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg03/terrain/draw_239.inc"
};
static Sint16 draw_59_polygons[219] = {
#include "stg03/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[199] = {
#include "stg03/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg03/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg03/terrain/draw_238.inc"
};
static Sint16 draw_237_polygons[195] = {
#include "stg03/terrain/draw_237_polygons.inc"
};
static Sint32 draw_237_vertices[223] = {
#include "stg03/terrain/draw_237_vertices.inc"
};
static NJS_CNK_MODEL draw_237_model = {
#include "stg03/terrain/draw_237_model.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg03/terrain/draw_237.inc"
};
static Sint16 draw_234_polygons[833] = {
#include "stg03/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[715] = {
#include "stg03/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg03/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg03/terrain/draw_236.inc"
};
static Sint16 draw_235_polygons[579] = {
#include "stg03/terrain/draw_235_polygons.inc"
};
static Sint32 draw_235_vertices[443] = {
#include "stg03/terrain/draw_235_vertices.inc"
};
static NJS_CNK_MODEL draw_235_model = {
#include "stg03/terrain/draw_235_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg03/terrain/draw_235.inc"
};
static Sint16 draw_172_polygons[273] = {
#include "stg03/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[207] = {
#include "stg03/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg03/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg03/terrain/draw_233.inc"
};
static Sint16 draw_155_polygons[579] = {
#include "stg03/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[555] = {
#include "stg03/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg03/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg03/terrain/draw_231.inc"
};
static Sint16 draw_158_polygons[3201] = {
#include "stg03/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[2951] = {
#include "stg03/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg03/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg03/terrain/draw_230.inc"
};
static Sint16 draw_229_polygons[263] = {
#include "stg03/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[191] = {
#include "stg03/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg03/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg03/terrain/draw_229.inc"
};
static Sint16 draw_228_polygons[359] = {
#include "stg03/terrain/draw_228_polygons.inc"
};
static Sint32 draw_228_vertices[243] = {
#include "stg03/terrain/draw_228_vertices.inc"
};
static NJS_CNK_MODEL draw_228_model = {
#include "stg03/terrain/draw_228_model.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg03/terrain/draw_228.inc"
};
static Sint16 draw_227_polygons[87] = {
#include "stg03/terrain/draw_227_polygons.inc"
};
static Sint32 draw_227_vertices[99] = {
#include "stg03/terrain/draw_227_vertices.inc"
};
static NJS_CNK_MODEL draw_227_model = {
#include "stg03/terrain/draw_227_model.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg03/terrain/draw_227.inc"
};
static Sint16 draw_226_polygons[201] = {
#include "stg03/terrain/draw_226_polygons.inc"
};
static Sint32 draw_226_vertices[167] = {
#include "stg03/terrain/draw_226_vertices.inc"
};
static NJS_CNK_MODEL draw_226_model = {
#include "stg03/terrain/draw_226_model.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg03/terrain/draw_226.inc"
};
static Sint16 draw_225_polygons[187] = {
#include "stg03/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[187] = {
#include "stg03/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg03/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg03/terrain/draw_225.inc"
};
static Sint16 draw_154_polygons[887] = {
#include "stg03/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[795] = {
#include "stg03/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg03/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg03/terrain/draw_223.inc"
};
static Sint16 draw_222_polygons[93] = {
#include "stg03/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[103] = {
#include "stg03/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg03/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg03/terrain/draw_222.inc"
};
static Sint16 draw_221_polygons[59] = {
#include "stg03/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[43] = {
#include "stg03/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg03/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg03/terrain/draw_221.inc"
};
static Sint16 draw_219_polygons[1463] = {
#include "stg03/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[1187] = {
#include "stg03/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg03/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg03/terrain/draw_219.inc"
};
static Sint16 draw_218_polygons[29] = {
#include "stg03/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[27] = {
#include "stg03/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg03/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg03/terrain/draw_218.inc"
};
static Sint16 draw_217_polygons[309] = {
#include "stg03/terrain/draw_217_polygons.inc"
};
static Sint32 draw_217_vertices[231] = {
#include "stg03/terrain/draw_217_vertices.inc"
};
static NJS_CNK_MODEL draw_217_model = {
#include "stg03/terrain/draw_217_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg03/terrain/draw_217.inc"
};
static Sint16 draw_216_polygons[75] = {
#include "stg03/terrain/draw_216_polygons.inc"
};
static Sint32 draw_216_vertices[75] = {
#include "stg03/terrain/draw_216_vertices.inc"
};
static NJS_CNK_MODEL draw_216_model = {
#include "stg03/terrain/draw_216_model.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg03/terrain/draw_216.inc"
};
static Sint16 draw_215_polygons[217] = {
#include "stg03/terrain/draw_215_polygons.inc"
};
static Sint32 draw_215_vertices[219] = {
#include "stg03/terrain/draw_215_vertices.inc"
};
static NJS_CNK_MODEL draw_215_model = {
#include "stg03/terrain/draw_215_model.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg03/terrain/draw_215.inc"
};
static Sint16 draw_214_polygons[121] = {
#include "stg03/terrain/draw_214_polygons.inc"
};
static Sint32 draw_214_vertices[115] = {
#include "stg03/terrain/draw_214_vertices.inc"
};
static NJS_CNK_MODEL draw_214_model = {
#include "stg03/terrain/draw_214_model.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg03/terrain/draw_214.inc"
};
static Sint16 draw_213_polygons[205] = {
#include "stg03/terrain/draw_213_polygons.inc"
};
static Sint32 draw_213_vertices[243] = {
#include "stg03/terrain/draw_213_vertices.inc"
};
static NJS_CNK_MODEL draw_213_model = {
#include "stg03/terrain/draw_213_model.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg03/terrain/draw_213.inc"
};
static Sint16 draw_212_polygons[115] = {
#include "stg03/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[107] = {
#include "stg03/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg03/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg03/terrain/draw_212.inc"
};
static Sint16 draw_211_polygons[127] = {
#include "stg03/terrain/draw_211_polygons.inc"
};
static Sint32 draw_211_vertices[111] = {
#include "stg03/terrain/draw_211_vertices.inc"
};
static NJS_CNK_MODEL draw_211_model = {
#include "stg03/terrain/draw_211_model.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg03/terrain/draw_211.inc"
};
static Sint16 draw_210_polygons[61] = {
#include "stg03/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[63] = {
#include "stg03/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg03/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg03/terrain/draw_210.inc"
};
static Sint16 draw_209_polygons[113] = {
#include "stg03/terrain/draw_209_polygons.inc"
};
static Sint32 draw_209_vertices[131] = {
#include "stg03/terrain/draw_209_vertices.inc"
};
static NJS_CNK_MODEL draw_209_model = {
#include "stg03/terrain/draw_209_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg03/terrain/draw_209.inc"
};
static Sint16 draw_208_polygons[29] = {
#include "stg03/terrain/draw_208_polygons.inc"
};
static Sint32 draw_208_vertices[27] = {
#include "stg03/terrain/draw_208_vertices.inc"
};
static NJS_CNK_MODEL draw_208_model = {
#include "stg03/terrain/draw_208_model.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg03/terrain/draw_208.inc"
};
static Sint16 draw_207_polygons[115] = {
#include "stg03/terrain/draw_207_polygons.inc"
};
static Sint32 draw_207_vertices[123] = {
#include "stg03/terrain/draw_207_vertices.inc"
};
static NJS_CNK_MODEL draw_207_model = {
#include "stg03/terrain/draw_207_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg03/terrain/draw_207.inc"
};
static Sint16 draw_206_polygons[217] = {
#include "stg03/terrain/draw_206_polygons.inc"
};
static Sint32 draw_206_vertices[239] = {
#include "stg03/terrain/draw_206_vertices.inc"
};
static NJS_CNK_MODEL draw_206_model = {
#include "stg03/terrain/draw_206_model.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg03/terrain/draw_206.inc"
};
static Sint16 draw_205_polygons[417] = {
#include "stg03/terrain/draw_205_polygons.inc"
};
static Sint32 draw_205_vertices[327] = {
#include "stg03/terrain/draw_205_vertices.inc"
};
static NJS_CNK_MODEL draw_205_model = {
#include "stg03/terrain/draw_205_model.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg03/terrain/draw_205.inc"
};
static Sint16 draw_199_polygons[207] = {
#include "stg03/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[219] = {
#include "stg03/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg03/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg03/terrain/draw_200.inc"
};
static Sint16 draw_198_polygons[339] = {
#include "stg03/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[355] = {
#include "stg03/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg03/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg03/terrain/draw_198.inc"
};
static Sint16 draw_195_polygons[75] = {
#include "stg03/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[67] = {
#include "stg03/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg03/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg03/terrain/draw_195.inc"
};
static Sint16 draw_194_polygons[37] = {
#include "stg03/terrain/draw_194_polygons.inc"
};
static Sint32 draw_194_vertices[31] = {
#include "stg03/terrain/draw_194_vertices.inc"
};
static NJS_CNK_MODEL draw_194_model = {
#include "stg03/terrain/draw_194_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg03/terrain/draw_194.inc"
};
static Sint16 draw_193_polygons[929] = {
#include "stg03/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[771] = {
#include "stg03/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg03/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg03/terrain/draw_193.inc"
};
static Sint16 draw_192_polygons[1011] = {
#include "stg03/terrain/draw_192_polygons.inc"
};
static Sint32 draw_192_vertices[811] = {
#include "stg03/terrain/draw_192_vertices.inc"
};
static NJS_CNK_MODEL draw_192_model = {
#include "stg03/terrain/draw_192_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg03/terrain/draw_192.inc"
};
static Sint16 draw_191_polygons[611] = {
#include "stg03/terrain/draw_191_polygons.inc"
};
static Sint32 draw_191_vertices[655] = {
#include "stg03/terrain/draw_191_vertices.inc"
};
static NJS_CNK_MODEL draw_191_model = {
#include "stg03/terrain/draw_191_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg03/terrain/draw_191.inc"
};
static Sint16 draw_190_polygons[279] = {
#include "stg03/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[231] = {
#include "stg03/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg03/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg03/terrain/draw_190.inc"
};
static Sint16 draw_189_polygons[229] = {
#include "stg03/terrain/draw_189_polygons.inc"
};
static Sint32 draw_189_vertices[207] = {
#include "stg03/terrain/draw_189_vertices.inc"
};
static NJS_CNK_MODEL draw_189_model = {
#include "stg03/terrain/draw_189_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg03/terrain/draw_189.inc"
};
static Sint16 draw_186_polygons[107] = {
#include "stg03/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[119] = {
#include "stg03/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg03/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg03/terrain/draw_186.inc"
};
static Sint16 draw_185_polygons[501] = {
#include "stg03/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[439] = {
#include "stg03/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg03/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg03/terrain/draw_185.inc"
};
static Sint16 draw_184_polygons[113] = {
#include "stg03/terrain/draw_184_polygons.inc"
};
static Sint32 draw_184_vertices[107] = {
#include "stg03/terrain/draw_184_vertices.inc"
};
static NJS_CNK_MODEL draw_184_model = {
#include "stg03/terrain/draw_184_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg03/terrain/draw_184.inc"
};
static Sint16 draw_183_polygons[49] = {
#include "stg03/terrain/draw_183_polygons.inc"
};
static Sint32 draw_183_vertices[47] = {
#include "stg03/terrain/draw_183_vertices.inc"
};
static NJS_CNK_MODEL draw_183_model = {
#include "stg03/terrain/draw_183_model.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg03/terrain/draw_183.inc"
};
static Sint16 draw_180_polygons[935] = {
#include "stg03/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[683] = {
#include "stg03/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg03/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg03/terrain/draw_180.inc"
};
static Sint16 draw_178_polygons[63] = {
#include "stg03/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[59] = {
#include "stg03/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg03/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg03/terrain/draw_178.inc"
};
static Sint16 draw_176_polygons[69] = {
#include "stg03/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[27] = {
#include "stg03/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg03/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg03/terrain/draw_177.inc"
};
static Sint16 draw_170_polygons[595] = {
#include "stg03/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[515] = {
#include "stg03/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg03/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg03/terrain/draw_170.inc"
};
static Sint16 draw_169_polygons[189] = {
#include "stg03/terrain/draw_169_polygons.inc"
};
static Sint32 draw_169_vertices[171] = {
#include "stg03/terrain/draw_169_vertices.inc"
};
static NJS_CNK_MODEL draw_169_model = {
#include "stg03/terrain/draw_169_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg03/terrain/draw_169.inc"
};
static Sint16 draw_168_polygons[1223] = {
#include "stg03/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[963] = {
#include "stg03/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg03/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg03/terrain/draw_168.inc"
};
static Sint16 draw_17_polygons[461] = {
#include "stg03/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[343] = {
#include "stg03/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg03/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg03/terrain/draw_160.inc"
};
static Sint16 draw_35_polygons[509] = {
#include "stg03/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[451] = {
#include "stg03/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg03/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg03/terrain/draw_159.inc"
};
static Sint16 draw_147_polygons[313] = {
#include "stg03/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[219] = {
#include "stg03/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg03/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg03/terrain/draw_147.inc"
};
static Sint16 draw_146_polygons[239] = {
#include "stg03/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[167] = {
#include "stg03/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg03/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg03/terrain/draw_146.inc"
};
static Sint16 draw_145_polygons[35] = {
#include "stg03/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[35] = {
#include "stg03/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg03/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg03/terrain/draw_145.inc"
};
static Sint16 draw_144_polygons[35] = {
#include "stg03/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[35] = {
#include "stg03/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg03/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg03/terrain/draw_144.inc"
};
static Sint16 draw_143_polygons[67] = {
#include "stg03/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[43] = {
#include "stg03/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg03/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg03/terrain/draw_143.inc"
};
static Sint16 draw_142_polygons[37] = {
#include "stg03/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[31] = {
#include "stg03/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg03/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg03/terrain/draw_142.inc"
};
static Sint16 draw_141_polygons[1125] = {
#include "stg03/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[863] = {
#include "stg03/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg03/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg03/terrain/draw_141.inc"
};
static Sint16 draw_140_polygons[193] = {
#include "stg03/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[175] = {
#include "stg03/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg03/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg03/terrain/draw_140.inc"
};
static Sint16 draw_139_polygons[201] = {
#include "stg03/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[187] = {
#include "stg03/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg03/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg03/terrain/draw_139.inc"
};
static Sint16 draw_138_polygons[131] = {
#include "stg03/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[111] = {
#include "stg03/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg03/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg03/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[91] = {
#include "stg03/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[79] = {
#include "stg03/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg03/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg03/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[239] = {
#include "stg03/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[143] = {
#include "stg03/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg03/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg03/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[99] = {
#include "stg03/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[75] = {
#include "stg03/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg03/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg03/terrain/draw_135.inc"
};
static Sint16 draw_134_polygons[445] = {
#include "stg03/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[359] = {
#include "stg03/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg03/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg03/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[631] = {
#include "stg03/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[447] = {
#include "stg03/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg03/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg03/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[91] = {
#include "stg03/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[55] = {
#include "stg03/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg03/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg03/terrain/draw_132.inc"
};
static Sint16 draw_43_polygons[63] = {
#include "stg03/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[55] = {
#include "stg03/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg03/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg03/terrain/draw_128.inc"
};
static Sint16 draw_39_polygons[133] = {
#include "stg03/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[123] = {
#include "stg03/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg03/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg03/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[209] = {
#include "stg03/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[179] = {
#include "stg03/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg03/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg03/terrain/draw_123.inc"
};
static Sint16 draw_121_polygons[331] = {
#include "stg03/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[283] = {
#include "stg03/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg03/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg03/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[1991] = {
#include "stg03/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[1743] = {
#include "stg03/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg03/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg03/terrain/draw_120.inc"
};
static Sint16 draw_113_polygons[393] = {
#include "stg03/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[395] = {
#include "stg03/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg03/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg03/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[69] = {
#include "stg03/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[27] = {
#include "stg03/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg03/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg03/terrain/draw_112.inc"
};
static Sint16 draw_109_polygons[63] = {
#include "stg03/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[55] = {
#include "stg03/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg03/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg03/terrain/draw_109.inc"
};
static Sint16 draw_108_polygons[349] = {
#include "stg03/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[263] = {
#include "stg03/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg03/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg03/terrain/draw_108.inc"
};
static Sint16 draw_107_polygons[1913] = {
#include "stg03/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[1731] = {
#include "stg03/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg03/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg03/terrain/draw_107.inc"
};
static Sint16 draw_3_polygons[601] = {
#include "stg03/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[447] = {
#include "stg03/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg03/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg03/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[417] = {
#include "stg03/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[311] = {
#include "stg03/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg03/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg03/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[297] = {
#include "stg03/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[183] = {
#include "stg03/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg03/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg03/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[265] = {
#include "stg03/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[191] = {
#include "stg03/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg03/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg03/terrain/draw_103.inc"
};
static Sint16 draw_29_polygons[565] = {
#include "stg03/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[399] = {
#include "stg03/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg03/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg03/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[607] = {
#include "stg03/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[475] = {
#include "stg03/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg03/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg03/terrain/draw_101.inc"
};
static Sint16 draw_99_polygons[245] = {
#include "stg03/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[263] = {
#include "stg03/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg03/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg03/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[49] = {
#include "stg03/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[43] = {
#include "stg03/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg03/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg03/terrain/draw_98.inc"
};
static Sint16 draw_97_polygons[461] = {
#include "stg03/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[387] = {
#include "stg03/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg03/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg03/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[163] = {
#include "stg03/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[103] = {
#include "stg03/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg03/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg03/terrain/draw_96.inc"
};
static Sint16 draw_81_polygons[695] = {
#include "stg03/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[599] = {
#include "stg03/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg03/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg03/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[127] = {
#include "stg03/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[111] = {
#include "stg03/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg03/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg03/terrain/draw_92.inc"
};
static Sint16 draw_32_polygons[3731] = {
#include "stg03/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[3215] = {
#include "stg03/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg03/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg03/terrain/draw_88.inc"
};
static Sint16 draw_14_polygons[69] = {
#include "stg03/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[51] = {
#include "stg03/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg03/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg03/terrain/draw_83.inc"
};
static Sint16 draw_80_polygons[259] = {
#include "stg03/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[207] = {
#include "stg03/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg03/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg03/terrain/draw_80.inc"
};
static Sint16 draw_12_polygons[139] = {
#include "stg03/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[123] = {
#include "stg03/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg03/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg03/terrain/draw_79.inc"
};
static Sint16 draw_77_polygons[69] = {
#include "stg03/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[67] = {
#include "stg03/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg03/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg03/terrain/draw_77.inc"
};
static Sint16 draw_74_polygons[89] = {
#include "stg03/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[91] = {
#include "stg03/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg03/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg03/terrain/draw_76.inc"
};
static Sint16 draw_75_polygons[49] = {
#include "stg03/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[43] = {
#include "stg03/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg03/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg03/terrain/draw_75.inc"
};
static Sint16 draw_73_polygons[61] = {
#include "stg03/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[63] = {
#include "stg03/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg03/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg03/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[693] = {
#include "stg03/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[563] = {
#include "stg03/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg03/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg03/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[2321] = {
#include "stg03/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[2087] = {
#include "stg03/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg03/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg03/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[1653] = {
#include "stg03/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[1479] = {
#include "stg03/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg03/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg03/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[207] = {
#include "stg03/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[167] = {
#include "stg03/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg03/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg03/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[1237] = {
#include "stg03/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[1015] = {
#include "stg03/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg03/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg03/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[439] = {
#include "stg03/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[259] = {
#include "stg03/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg03/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg03/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[271] = {
#include "stg03/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[223] = {
#include "stg03/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg03/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg03/terrain/draw_66.inc"
};
static Sint16 draw_64_polygons[117] = {
#include "stg03/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[91] = {
#include "stg03/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg03/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg03/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[239] = {
#include "stg03/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[223] = {
#include "stg03/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg03/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg03/terrain/draw_63.inc"
};
static Sint16 draw_58_polygons[153] = {
#include "stg03/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[171] = {
#include "stg03/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg03/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg03/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[23] = {
#include "stg03/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[19] = {
#include "stg03/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg03/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg03/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[23] = {
#include "stg03/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[19] = {
#include "stg03/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg03/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg03/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[23] = {
#include "stg03/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[19] = {
#include "stg03/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg03/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg03/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[23] = {
#include "stg03/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[19] = {
#include "stg03/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg03/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg03/terrain/draw_54.inc"
};
static Sint16 draw_1_polygons[455] = {
#include "stg03/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[399] = {
#include "stg03/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg03/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg03/terrain/draw_53.inc"
};
static Sint16 draw_0_polygons[547] = {
#include "stg03/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[463] = {
#include "stg03/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg03/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg03/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[401] = {
#include "stg03/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[247] = {
#include "stg03/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg03/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg03/terrain/draw_51.inc"
};
static Sint16 draw_47_polygons[357] = {
#include "stg03/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[339] = {
#include "stg03/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg03/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg03/terrain/draw_50.inc"
};
static Sint16 draw_34_polygons[495] = {
#include "stg03/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[415] = {
#include "stg03/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg03/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg03/terrain/draw_34.inc"
};
static Sint16 draw_31_polygons[1779] = {
#include "stg03/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[1407] = {
#include "stg03/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg03/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg03/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[177] = {
#include "stg03/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[155] = {
#include "stg03/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg03/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg03/terrain/draw_30.inc"
};
static Sint16 draw_25_polygons[383] = {
#include "stg03/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[295] = {
#include "stg03/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg03/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg03/terrain/draw_25.inc"
};
static Sint16 draw_22_polygons[1825] = {
#include "stg03/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[1651] = {
#include "stg03/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg03/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg03/terrain/draw_24.inc"
};
static Sint16 draw_13_polygons[311] = {
#include "stg03/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[235] = {
#include "stg03/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg03/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg03/terrain/draw_13.inc"
};
static Sint16 draw_11_polygons[101] = {
#include "stg03/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[103] = {
#include "stg03/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg03/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg03/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[229] = {
#include "stg03/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[219] = {
#include "stg03/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg03/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg03/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[67] = {
#include "stg03/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[47] = {
#include "stg03/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg03/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg03/terrain/draw_9.inc"
};
static Sint16 draw_7_polygons[265] = {
#include "stg03/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[191] = {
#include "stg03/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg03/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg03/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[189] = {
#include "stg03/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[115] = {
#include "stg03/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg03/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg03/terrain/draw_6.inc"
};
static Sint16 draw_4_polygons[13] = {
#include "stg03/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[19] = {
#include "stg03/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg03/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg03/terrain/draw_4.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg03/terrain/collision_4.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg03/terrain/collision_23.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg03/terrain/collision_25.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg03/terrain/collision_27.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg03/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg03/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg03/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg03/terrain/collision_47.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg03/terrain/collision_54.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg03/terrain/collision_87.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg03/terrain/collision_92.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg03/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg03/terrain/collision_96.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg03/terrain/collision_103.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg03/terrain/collision_105.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg03/terrain/collision_118.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg03/terrain/collision_119.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg03/terrain/collision_120.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg03/terrain/collision_133.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg03/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg03/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg03/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg03/terrain/collision_143.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg03/terrain/collision_144.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg03/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg03/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg03/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg03/terrain/collision_148.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg03/terrain/collision_154.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg03/terrain/collision_156.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg03/terrain/collision_157.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg03/terrain/collision_158.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg03/terrain/collision_159.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg03/terrain/collision_160.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg03/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg03/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg03/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg03/terrain/collision_166.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg03/terrain/collision_167.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg03/terrain/collision_177.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg03/terrain/collision_178.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg03/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg03/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg03/terrain/collision_181.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg03/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg03/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg03/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg03/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg03/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg03/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg03/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg03/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg03/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg03/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg03/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg03/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg03/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg03/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg03/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg03/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg03/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg03/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg03/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg03/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg03/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg03/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg03/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg03/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg03/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg03/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg03/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg03/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg03/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg03/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg03/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg03/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg03/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg03/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg03/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg03/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg03/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg03/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg03/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg03/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg03/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg03/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg03/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg03/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg03/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg03/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg03/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg03/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg03/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg03/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg03/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg03/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg03/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg03/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg03/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg03/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg03/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg03/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg03/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg03/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg03/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg03/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg03/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg03/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg03/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg03/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg03/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg03/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg03/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg03/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg03/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg03/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg03/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg03/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg03/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg03/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg03/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg03/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg03/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg03/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg03/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg03/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg03/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg03/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg03/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg03/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg03/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg03/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg03/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg03/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg03/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg03/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg03/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg03/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg03/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg03/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg03/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg03/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg03/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg03/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg03/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg03/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg03/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg03/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg03/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg03/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg03/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg03/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg03/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg03/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg03/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg03/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg03/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg03/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg03/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg03/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg03/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg03/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg03/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg03/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg03/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg03/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg03/terrain/draw_0.inc"
};
