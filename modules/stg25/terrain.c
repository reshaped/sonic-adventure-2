#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg25/death_chamber.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char death_chamber_land_filenames[691] = {
#include "stg25/terrain/death_chamber_land_filenames.inc"
};

/* STG25.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between death_chamber.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[517];
struct land_table death_chamber_land;
static NJS_TEXNAME textures[45];
NJS_TEXLIST death_chamber_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[29];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[16];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[41];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[20];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[69];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[20];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[46];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[16];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[61];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[22];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[64];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[22];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[39];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[12];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[33];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[13];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[25];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[20];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[225];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[72];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[16];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[8];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[25];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[12];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[85];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[24];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[85];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[24];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[70];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[24];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[15];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[8];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[48];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[24];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[111];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[44];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[103];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[48];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[54];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[24];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[23];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[12];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[150];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[64];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[37];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[24];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[172];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[64];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[111];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[48];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[106];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[44];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[15];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[12];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[325];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[84];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[116];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[52];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[30];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[24];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[14];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[8];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[14];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[8];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[34];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[32];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[103];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[44];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[85];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[68];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[45];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[36];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[50];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[24];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[50];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[24];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[15];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[8];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[66];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[27];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[28];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[12];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[25];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[12];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[39];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[26];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[54];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[24];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[20];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[8];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[20];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[8];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[20];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[8];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[25];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[8];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[39];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[14];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[56];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[32];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_96_materials[1];
static Sint16 collision_96_faces[25];
static NJS_MESHSET collision_96_meshsets[1];
static NJS_POINT3 collision_96_points[8];
static NJS_MODEL collision_96_model;
static NJS_OBJECT collision_96;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[108];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[44];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[37];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[12];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[119];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[42];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[15];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[8];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[43];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[18];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_105_materials[1];
static Sint16 collision_105_faces[28];
static NJS_MESHSET collision_105_meshsets[1];
static NJS_POINT3 collision_105_points[12];
static NJS_MODEL collision_105_model;
static NJS_OBJECT collision_105;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[25];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[12];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[16];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[8];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[39];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[14];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[56];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[32];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[25];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[8];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[32];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[14];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[15];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[8];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[51];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[20];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[25];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[12];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[28];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[12];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[30];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[20];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[30];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[12];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[5];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[4];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_126_materials[1];
static Sint16 collision_126_faces[125];
static NJS_MESHSET collision_126_meshsets[1];
static NJS_POINT3 collision_126_points[46];
static NJS_MODEL collision_126_model;
static NJS_OBJECT collision_126;
static NJS_MATERIAL collision_128_materials[1];
static Sint16 collision_128_faces[5];
static NJS_MESHSET collision_128_meshsets[1];
static NJS_POINT3 collision_128_points[4];
static NJS_MODEL collision_128_model;
static NJS_OBJECT collision_128;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[145];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[46];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[15];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[8];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[15];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[8];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_132_materials[1];
static Sint16 collision_132_faces[15];
static NJS_MESHSET collision_132_meshsets[1];
static NJS_POINT3 collision_132_points[8];
static NJS_MODEL collision_132_model;
static NJS_OBJECT collision_132;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[309];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[128];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[22];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[12];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[99];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[52];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[142];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[56];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[211];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[78];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[46];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[22];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[49];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[22];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[76];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[44];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[40];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[18];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[12];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[7];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[360];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[131];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[374];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[148];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[218];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[80];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[11];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[8];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[118];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[40];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[118];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[40];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[27];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[12];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_181_materials[1];
static Sint16 collision_181_faces[32];
static NJS_MESHSET collision_181_meshsets[1];
static NJS_POINT3 collision_181_points[12];
static NJS_MODEL collision_181_model;
static NJS_OBJECT collision_181;
static NJS_MATERIAL collision_183_materials[1];
static Sint16 collision_183_faces[32];
static NJS_MESHSET collision_183_meshsets[1];
static NJS_POINT3 collision_183_points[12];
static NJS_MODEL collision_183_model;
static NJS_OBJECT collision_183;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[34];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[14];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[156];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[70];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[44];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[20];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_195_materials[1];
static Sint16 collision_195_faces[45];
static NJS_MESHSET collision_195_meshsets[1];
static NJS_POINT3 collision_195_points[18];
static NJS_MODEL collision_195_model;
static NJS_OBJECT collision_195;
static NJS_MATERIAL collision_198_materials[1];
static Sint16 collision_198_faces[32];
static NJS_MESHSET collision_198_meshsets[1];
static NJS_POINT3 collision_198_points[14];
static NJS_MODEL collision_198_model;
static NJS_OBJECT collision_198;
static NJS_MATERIAL collision_201_materials[1];
static Sint16 collision_201_faces[29];
static NJS_MESHSET collision_201_meshsets[1];
static NJS_POINT3 collision_201_points[12];
static NJS_MODEL collision_201_model;
static NJS_OBJECT collision_201;
static NJS_MATERIAL collision_202_materials[1];
static Sint16 collision_202_faces[32];
static NJS_MESHSET collision_202_meshsets[1];
static NJS_POINT3 collision_202_points[16];
static NJS_MODEL collision_202_model;
static NJS_OBJECT collision_202;
static NJS_MATERIAL collision_203_materials[1];
static Sint16 collision_203_faces[33];
static NJS_MESHSET collision_203_meshsets[1];
static NJS_POINT3 collision_203_points[15];
static NJS_MODEL collision_203_model;
static NJS_OBJECT collision_203;
static NJS_MATERIAL collision_204_materials[1];
static Sint16 collision_204_faces[12];
static NJS_MESHSET collision_204_meshsets[1];
static NJS_POINT3 collision_204_points[8];
static NJS_MODEL collision_204_model;
static NJS_OBJECT collision_204;
static NJS_MATERIAL collision_205_materials[1];
static Sint16 collision_205_faces[12];
static NJS_MESHSET collision_205_meshsets[1];
static NJS_POINT3 collision_205_points[8];
static NJS_MODEL collision_205_model;
static NJS_OBJECT collision_205;
static NJS_MATERIAL collision_206_materials[1];
static Sint16 collision_206_faces[18];
static NJS_MESHSET collision_206_meshsets[1];
static NJS_POINT3 collision_206_points[12];
static NJS_MODEL collision_206_model;
static NJS_OBJECT collision_206;
static NJS_MATERIAL collision_212_materials[1];
static Sint16 collision_212_faces[96];
static NJS_MESHSET collision_212_meshsets[1];
static NJS_POINT3 collision_212_points[56];
static NJS_MODEL collision_212_model;
static NJS_OBJECT collision_212;
static NJS_MATERIAL collision_215_materials[1];
static Sint16 collision_215_faces[18];
static NJS_MESHSET collision_215_meshsets[1];
static NJS_POINT3 collision_215_points[12];
static NJS_MODEL collision_215_model;
static NJS_OBJECT collision_215;
static NJS_MATERIAL collision_221_materials[1];
static Sint16 collision_221_faces[47];
static NJS_MESHSET collision_221_meshsets[1];
static NJS_POINT3 collision_221_points[25];
static NJS_MODEL collision_221_model;
static NJS_OBJECT collision_221;
static NJS_MATERIAL collision_222_materials[1];
static Sint16 collision_222_faces[32];
static NJS_MESHSET collision_222_meshsets[1];
static NJS_POINT3 collision_222_points[12];
static NJS_MODEL collision_222_model;
static NJS_OBJECT collision_222;
static NJS_MATERIAL collision_224_materials[1];
static Sint16 collision_224_faces[32];
static NJS_MESHSET collision_224_meshsets[1];
static NJS_POINT3 collision_224_points[12];
static NJS_MODEL collision_224_model;
static NJS_OBJECT collision_224;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[94];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[35];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_226_materials[1];
static Sint16 collision_226_faces[27];
static NJS_MESHSET collision_226_meshsets[1];
static NJS_POINT3 collision_226_points[11];
static NJS_MODEL collision_226_model;
static NJS_OBJECT collision_226;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[27];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[11];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_228_materials[1];
static Sint16 collision_228_faces[9];
static NJS_MESHSET collision_228_meshsets[1];
static NJS_POINT3 collision_228_points[8];
static NJS_MODEL collision_228_model;
static NJS_OBJECT collision_228;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[45];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[24];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[32];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[12];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_235_materials[1];
static Sint16 collision_235_faces[32];
static NJS_MESHSET collision_235_meshsets[1];
static NJS_POINT3 collision_235_points[12];
static NJS_MODEL collision_235_model;
static NJS_OBJECT collision_235;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[141];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[46];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_237_materials[1];
static Sint16 collision_237_faces[28];
static NJS_MESHSET collision_237_meshsets[1];
static NJS_POINT3 collision_237_points[11];
static NJS_MODEL collision_237_model;
static NJS_OBJECT collision_237;
static NJS_MATERIAL collision_238_materials[1];
static Sint16 collision_238_faces[28];
static NJS_MESHSET collision_238_meshsets[1];
static NJS_POINT3 collision_238_points[11];
static NJS_MODEL collision_238_model;
static NJS_OBJECT collision_238;
static NJS_MATERIAL collision_239_materials[1];
static Sint16 collision_239_faces[54];
static NJS_MESHSET collision_239_meshsets[1];
static NJS_POINT3 collision_239_points[23];
static NJS_MODEL collision_239_model;
static NJS_OBJECT collision_239;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[34];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[19];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_241_materials[1];
static Sint16 collision_241_faces[25];
static NJS_MESHSET collision_241_meshsets[1];
static NJS_POINT3 collision_241_points[12];
static NJS_MODEL collision_241_model;
static NJS_OBJECT collision_241;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[31];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[12];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_243_materials[1];
static Sint16 collision_243_faces[49];
static NJS_MESHSET collision_243_meshsets[1];
static NJS_POINT3 collision_243_points[20];
static NJS_MODEL collision_243_model;
static NJS_OBJECT collision_243;
static NJS_MATERIAL collision_244_materials[1];
static Sint16 collision_244_faces[23];
static NJS_MESHSET collision_244_meshsets[1];
static NJS_POINT3 collision_244_points[16];
static NJS_MODEL collision_244_model;
static NJS_OBJECT collision_244;
static NJS_MATERIAL collision_246_materials[1];
static Sint16 collision_246_faces[7];
static NJS_MESHSET collision_246_meshsets[1];
static NJS_POINT3 collision_246_points[6];
static NJS_MODEL collision_246_model;
static NJS_OBJECT collision_246;
static NJS_MATERIAL collision_247_materials[1];
static Sint16 collision_247_faces[7];
static NJS_MESHSET collision_247_meshsets[1];
static NJS_POINT3 collision_247_points[6];
static NJS_MODEL collision_247_model;
static NJS_OBJECT collision_247;
static NJS_MATERIAL collision_248_materials[1];
static Sint16 collision_248_faces[94];
static NJS_MESHSET collision_248_meshsets[1];
static NJS_POINT3 collision_248_points[46];
static NJS_MODEL collision_248_model;
static NJS_OBJECT collision_248;
static NJS_MATERIAL collision_249_materials[1];
static Sint16 collision_249_faces[39];
static NJS_MESHSET collision_249_meshsets[1];
static NJS_POINT3 collision_249_points[17];
static NJS_MODEL collision_249_model;
static NJS_OBJECT collision_249;
static NJS_MATERIAL collision_250_materials[1];
static Sint16 collision_250_faces[5];
static NJS_MESHSET collision_250_meshsets[1];
static NJS_POINT3 collision_250_points[4];
static NJS_MODEL collision_250_model;
static NJS_OBJECT collision_250;
static NJS_MATERIAL collision_251_materials[1];
static Sint16 collision_251_faces[5];
static NJS_MESHSET collision_251_meshsets[1];
static NJS_POINT3 collision_251_points[4];
static NJS_MODEL collision_251_model;
static NJS_OBJECT collision_251;
static Sint16 draw_263_polygons[45];
static Sint32 draw_263_vertices[39];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_263;
static Sint16 draw_259_polygons[103];
static Sint32 draw_259_vertices[75];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_262;
static Sint16 draw_260_polygons[779];
static Sint32 draw_260_vertices[391];
static NJS_CNK_MODEL draw_260_model;
static NJS_CNK_OBJECT draw_260;
static Sint16 draw_258_polygons[783];
static Sint32 draw_258_vertices[671];
static NJS_CNK_MODEL draw_258_model;
static NJS_CNK_OBJECT draw_258;
static Sint16 draw_257_polygons[675];
static Sint32 draw_257_vertices[331];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_257;
static Sint16 draw_256_polygons[663];
static Sint32 draw_256_vertices[315];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_256;
static Sint16 draw_255_polygons[783];
static Sint32 draw_255_vertices[659];
static NJS_CNK_MODEL draw_255_model;
static NJS_CNK_OBJECT draw_255;
static Sint16 draw_253_polygons[927];
static Sint32 draw_253_vertices[851];
static NJS_CNK_MODEL draw_253_model;
static NJS_CNK_OBJECT draw_254;
static Sint16 draw_252_polygons[519];
static Sint32 draw_252_vertices[527];
static NJS_CNK_MODEL draw_252_model;
static NJS_CNK_OBJECT draw_252;
static Sint16 draw_251_polygons[265];
static Sint32 draw_251_vertices[195];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_251;
static Sint16 draw_250_polygons[195];
static Sint32 draw_250_vertices[219];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_249_polygons[405];
static Sint32 draw_249_vertices[227];
static NJS_CNK_MODEL draw_249_model;
static NJS_CNK_OBJECT draw_249;
static Sint16 draw_248_polygons[249];
static Sint32 draw_248_vertices[183];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_247_polygons[71];
static Sint32 draw_247_vertices[39];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_246_polygons[289];
static Sint32 draw_246_vertices[131];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_245_polygons[67];
static Sint32 draw_245_vertices[55];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_244_polygons[329];
static Sint32 draw_244_vertices[231];
static NJS_CNK_MODEL draw_244_model;
static NJS_CNK_OBJECT draw_244;
static Sint16 draw_243_polygons[83];
static Sint32 draw_243_vertices[67];
static NJS_CNK_MODEL draw_243_model;
static NJS_CNK_OBJECT draw_243;
static Sint16 draw_242_polygons[115];
static Sint32 draw_242_vertices[83];
static NJS_CNK_MODEL draw_242_model;
static NJS_CNK_OBJECT draw_242;
static Sint16 draw_240_polygons[787];
static Sint32 draw_240_vertices[403];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_241;
static Sint16 draw_236_polygons[65];
static Sint32 draw_236_vertices[51];
static NJS_CNK_MODEL draw_236_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_234_polygons[217];
static Sint32 draw_234_vertices[143];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_230_polygons[121];
static Sint32 draw_230_vertices[71];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_227_polygons[89];
static Sint32 draw_227_vertices[75];
static NJS_CNK_MODEL draw_227_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_219_polygons[89];
static Sint32 draw_219_vertices[59];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_226;
static Sint16 draw_210_polygons[57];
static Sint32 draw_210_vertices[43];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_208_polygons[109];
static Sint32 draw_208_vertices[67];
static NJS_CNK_MODEL draw_208_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_204_polygons[65];
static Sint32 draw_204_vertices[51];
static NJS_CNK_MODEL draw_204_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_202_polygons[45];
static Sint32 draw_202_vertices[43];
static NJS_CNK_MODEL draw_202_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_200_polygons[343];
static Sint32 draw_200_vertices[163];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_198_polygons[339];
static Sint32 draw_198_vertices[195];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_196_polygons[331];
static Sint32 draw_196_vertices[163];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_195_polygons[255];
static Sint32 draw_195_vertices[107];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_194_polygons[307];
static Sint32 draw_194_vertices[171];
static NJS_CNK_MODEL draw_194_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_192_polygons[845];
static Sint32 draw_192_vertices[699];
static NJS_CNK_MODEL draw_192_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_190_polygons[189];
static Sint32 draw_190_vertices[195];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_188_polygons[165];
static Sint32 draw_188_vertices[195];
static NJS_CNK_MODEL draw_188_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_187_polygons[555];
static Sint32 draw_187_vertices[239];
static NJS_CNK_MODEL draw_187_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_183_polygons[57];
static Sint32 draw_183_vertices[43];
static NJS_CNK_MODEL draw_183_model;
static NJS_CNK_OBJECT draw_186;
static Sint16 draw_182_polygons[575];
static Sint32 draw_182_vertices[247];
static NJS_CNK_MODEL draw_182_model;
static NJS_CNK_OBJECT draw_182;
static Sint16 draw_178_polygons[57];
static Sint32 draw_178_vertices[43];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_181;
static Sint16 draw_170_polygons[61];
static Sint32 draw_170_vertices[43];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_169_polygons[63];
static Sint32 draw_169_vertices[35];
static NJS_CNK_MODEL draw_169_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_168_polygons[81];
static Sint32 draw_168_vertices[59];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_167_polygons[1341];
static Sint32 draw_167_vertices[1015];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_163_polygons[51];
static Sint32 draw_163_vertices[35];
static NJS_CNK_MODEL draw_163_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_160_polygons[141];
static Sint32 draw_160_vertices[123];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_162;
static Sint16 draw_159_polygons[101];
static Sint32 draw_159_vertices[67];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_151_polygons[63];
static Sint32 draw_151_vertices[59];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_149_polygons[51];
static Sint32 draw_149_vertices[51];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_148_polygons[343];
static Sint32 draw_148_vertices[159];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_147_polygons[1195];
static Sint32 draw_147_vertices[571];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_145_polygons[75];
static Sint32 draw_145_vertices[51];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_143_polygons[57];
static Sint32 draw_143_vertices[51];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_142_polygons[1195];
static Sint32 draw_142_vertices[567];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_141_polygons[85];
static Sint32 draw_141_vertices[63];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_141;
static Sint16 draw_140_polygons[81];
static Sint32 draw_140_vertices[59];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_139_polygons[81];
static Sint32 draw_139_vertices[59];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_138_polygons[209];
static Sint32 draw_138_vertices[151];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_137_polygons[219];
static Sint32 draw_137_vertices[131];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[219];
static Sint32 draw_136_vertices[131];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_132_polygons[269];
static Sint32 draw_132_vertices[143];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_131_polygons[263];
static Sint32 draw_131_vertices[147];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_128_polygons[269];
static Sint32 draw_128_vertices[147];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_127_polygons[173];
static Sint32 draw_127_vertices[103];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[235];
static Sint32 draw_126_vertices[227];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[255];
static Sint32 draw_125_vertices[159];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[229];
static Sint32 draw_124_vertices[219];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[261];
static Sint32 draw_123_vertices[159];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_122_polygons[229];
static Sint32 draw_122_vertices[223];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[83];
static Sint32 draw_121_vertices[55];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[229];
static Sint32 draw_120_vertices[223];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_119_polygons[121];
static Sint32 draw_119_vertices[67];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_118_polygons[1003];
static Sint32 draw_118_vertices[827];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_117_polygons[369];
static Sint32 draw_117_vertices[279];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_116_polygons[369];
static Sint32 draw_116_vertices[279];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[361];
static Sint32 draw_115_vertices[279];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[375];
static Sint32 draw_114_vertices[279];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_113_polygons[95];
static Sint32 draw_113_vertices[71];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_112_polygons[155];
static Sint32 draw_112_vertices[143];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_111_polygons[93];
static Sint32 draw_111_vertices[83];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_110_polygons[165];
static Sint32 draw_110_vertices[147];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_109_polygons[385];
static Sint32 draw_109_vertices[335];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_108_polygons[141];
static Sint32 draw_108_vertices[83];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_107_polygons[2789];
static Sint32 draw_107_vertices[1539];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_103_polygons[1013];
static Sint32 draw_103_vertices[599];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_102_polygons[337];
static Sint32 draw_102_vertices[235];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[57];
static Sint32 draw_101_vertices[39];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[327];
static Sint32 draw_100_vertices[323];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[147];
static Sint32 draw_99_vertices[99];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[147];
static Sint32 draw_98_vertices[99];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_97_polygons[169];
static Sint32 draw_97_vertices[99];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[147];
static Sint32 draw_96_vertices[99];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[663];
static Sint32 draw_95_vertices[411];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[247];
static Sint32 draw_94_vertices[203];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[395];
static Sint32 draw_93_vertices[323];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[409];
static Sint32 draw_92_vertices[371];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_90_polygons[341];
static Sint32 draw_90_vertices[147];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_88_polygons[307];
static Sint32 draw_88_vertices[171];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_86_polygons[263];
static Sint32 draw_86_vertices[107];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_85_polygons[317];
static Sint32 draw_85_vertices[135];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_84_polygons[181];
static Sint32 draw_84_vertices[127];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_83_polygons[471];
static Sint32 draw_83_vertices[279];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_82_polygons[559];
static Sint32 draw_82_vertices[299];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_82;
static Sint16 draw_81_polygons[153];
static Sint32 draw_81_vertices[83];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[83];
static Sint32 draw_80_vertices[75];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[117];
static Sint32 draw_79_vertices[99];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_78_polygons[203];
static Sint32 draw_78_vertices[99];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_77_polygons[861];
static Sint32 draw_77_vertices[479];
static NJS_CNK_MODEL draw_77_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_76_polygons[197];
static Sint32 draw_76_vertices[107];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_76;
static Sint16 draw_75_polygons[135];
static Sint32 draw_75_vertices[75];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_74_polygons[133];
static Sint32 draw_74_vertices[71];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_73_polygons[181];
static Sint32 draw_73_vertices[99];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[161];
static Sint32 draw_72_vertices[131];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[203];
static Sint32 draw_71_vertices[99];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[181];
static Sint32 draw_70_vertices[99];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[245];
static Sint32 draw_69_vertices[179];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[245];
static Sint32 draw_68_vertices[179];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[485];
static Sint32 draw_67_vertices[367];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[359];
static Sint32 draw_66_vertices[199];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[147];
static Sint32 draw_65_vertices[99];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[169];
static Sint32 draw_64_vertices[99];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[147];
static Sint32 draw_63_vertices[99];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[769];
static Sint32 draw_62_vertices[475];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[195];
static Sint32 draw_61_vertices[139];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[395];
static Sint32 draw_60_vertices[315];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[423];
static Sint32 draw_59_vertices[379];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_57_polygons[255];
static Sint32 draw_57_vertices[91];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_56_polygons[521];
static Sint32 draw_56_vertices[291];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[153];
static Sint32 draw_55_vertices[83];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[83];
static Sint32 draw_54_vertices[67];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[103];
static Sint32 draw_53_vertices[91];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[189];
static Sint32 draw_52_vertices[99];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[869];
static Sint32 draw_51_vertices[491];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[309];
static Sint32 draw_50_vertices[167];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_49_polygons[133];
static Sint32 draw_49_vertices[71];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_48_polygons[189];
static Sint32 draw_48_vertices[103];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_47_polygons[161];
static Sint32 draw_47_vertices[139];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_46_polygons[189];
static Sint32 draw_46_vertices[103];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_45_polygons[189];
static Sint32 draw_45_vertices[99];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_44_polygons[331];
static Sint32 draw_44_vertices[155];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[193];
static Sint32 draw_43_vertices[139];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[705];
static Sint32 draw_42_vertices[455];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[153];
static Sint32 draw_41_vertices[79];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[147];
static Sint32 draw_40_vertices[99];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_38_polygons[147];
static Sint32 draw_38_vertices[99];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_36_polygons[147];
static Sint32 draw_36_vertices[99];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_34_polygons[189];
static Sint32 draw_34_vertices[99];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_32_polygons[147];
static Sint32 draw_32_vertices[99];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[715];
static Sint32 draw_31_vertices[443];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[247];
static Sint32 draw_30_vertices[203];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[395];
static Sint32 draw_29_vertices[311];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[421];
static Sint32 draw_28_vertices[379];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_25_polygons[255];
static Sint32 draw_25_vertices[91];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_24_polygons[341];
static Sint32 draw_24_vertices[139];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[515];
static Sint32 draw_23_vertices[243];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_21_polygons[57];
static Sint32 draw_21_vertices[51];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_19_polygons[57];
static Sint32 draw_19_vertices[47];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_18_polygons[915];
static Sint32 draw_18_vertices[483];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[105];
static Sint32 draw_17_vertices[79];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[139];
static Sint32 draw_16_vertices[107];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[257];
static Sint32 draw_15_vertices[179];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[41];
static Sint32 draw_14_vertices[27];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[521];
static Sint32 draw_13_vertices[299];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[153];
static Sint32 draw_12_vertices[83];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[83];
static Sint32 draw_11_vertices[67];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[103];
static Sint32 draw_10_vertices[79];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[183];
static Sint32 draw_9_vertices[99];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[929];
static Sint32 draw_8_vertices[507];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[133];
static Sint32 draw_7_vertices[71];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[301];
static Sint32 draw_6_vertices[167];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[181];
static Sint32 draw_5_vertices[99];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[161];
static Sint32 draw_4_vertices[147];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[175];
static Sint32 draw_3_vertices[99];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[181];
static Sint32 draw_2_vertices[99];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[727];
static Sint32 draw_1_vertices[463];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[257];
static Sint32 draw_0_vertices[187];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_0;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_88;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_158;
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
static NJS_OBJECT collision_182;
static NJS_OBJECT collision_186;
static NJS_OBJECT collision_187;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_190;
static NJS_OBJECT collision_191;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_194;
static NJS_OBJECT collision_196;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_199;
static NJS_OBJECT collision_200;
static NJS_OBJECT collision_207;
static NJS_OBJECT collision_208;
static NJS_OBJECT collision_209;
static NJS_OBJECT collision_210;
static NJS_OBJECT collision_211;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_218;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_220;
static NJS_OBJECT collision_223;
static NJS_OBJECT collision_229;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_231;
static NJS_OBJECT collision_232;
static NJS_OBJECT collision_245;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_261;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_238;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_231;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_217;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_180;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_165;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_155;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_19;

static struct land_entry entries[517] = {
#include "stg25/terrain/entries.inc"
};
struct land_table death_chamber_land = {
#include "stg25/terrain/death_chamber_land.inc"
};
static NJS_TEXNAME textures[45] = {
#include "stg25/terrain/textures.inc"
};
NJS_TEXLIST death_chamber_land_texlist = {
#include "stg25/terrain/death_chamber_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg25/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[29] = {
#include "stg25/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg25/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[16] = {
#include "stg25/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg25/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg25/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg25/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[41] = {
#include "stg25/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg25/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[20] = {
#include "stg25/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg25/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg25/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg25/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[69] = {
#include "stg25/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg25/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[20] = {
#include "stg25/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg25/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg25/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg25/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[46] = {
#include "stg25/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg25/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[16] = {
#include "stg25/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg25/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg25/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg25/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[61] = {
#include "stg25/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg25/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[22] = {
#include "stg25/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg25/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg25/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg25/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[64] = {
#include "stg25/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg25/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[22] = {
#include "stg25/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg25/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg25/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg25/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[39] = {
#include "stg25/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg25/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[12] = {
#include "stg25/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg25/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg25/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg25/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[33] = {
#include "stg25/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg25/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[13] = {
#include "stg25/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg25/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg25/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg25/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[25] = {
#include "stg25/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg25/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[20] = {
#include "stg25/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg25/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg25/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg25/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[225] = {
#include "stg25/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg25/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[72] = {
#include "stg25/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg25/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg25/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg25/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[16] = {
#include "stg25/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg25/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[8] = {
#include "stg25/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg25/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg25/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg25/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[25] = {
#include "stg25/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg25/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[12] = {
#include "stg25/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg25/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg25/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg25/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[85] = {
#include "stg25/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg25/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[24] = {
#include "stg25/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg25/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg25/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg25/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[85] = {
#include "stg25/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg25/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[24] = {
#include "stg25/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg25/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg25/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg25/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[70] = {
#include "stg25/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg25/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[24] = {
#include "stg25/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg25/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg25/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg25/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[15] = {
#include "stg25/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg25/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[8] = {
#include "stg25/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg25/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg25/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg25/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[48] = {
#include "stg25/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg25/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[24] = {
#include "stg25/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg25/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg25/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg25/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[111] = {
#include "stg25/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg25/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[44] = {
#include "stg25/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg25/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg25/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg25/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[103] = {
#include "stg25/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg25/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[48] = {
#include "stg25/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg25/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg25/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg25/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[54] = {
#include "stg25/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg25/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[24] = {
#include "stg25/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg25/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg25/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg25/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[23] = {
#include "stg25/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg25/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[12] = {
#include "stg25/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg25/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg25/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg25/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[150] = {
#include "stg25/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg25/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[64] = {
#include "stg25/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg25/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg25/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg25/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[37] = {
#include "stg25/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg25/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[24] = {
#include "stg25/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg25/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg25/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg25/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[172] = {
#include "stg25/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg25/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[64] = {
#include "stg25/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg25/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg25/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg25/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[111] = {
#include "stg25/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg25/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[48] = {
#include "stg25/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg25/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg25/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg25/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[106] = {
#include "stg25/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg25/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[44] = {
#include "stg25/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg25/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg25/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg25/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[15] = {
#include "stg25/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg25/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[12] = {
#include "stg25/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg25/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg25/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg25/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[325] = {
#include "stg25/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg25/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[84] = {
#include "stg25/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg25/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg25/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg25/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[116] = {
#include "stg25/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg25/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[52] = {
#include "stg25/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg25/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg25/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg25/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[30] = {
#include "stg25/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg25/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[24] = {
#include "stg25/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg25/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg25/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg25/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[14] = {
#include "stg25/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg25/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[8] = {
#include "stg25/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg25/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg25/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg25/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[14] = {
#include "stg25/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg25/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[8] = {
#include "stg25/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg25/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg25/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg25/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[34] = {
#include "stg25/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg25/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[32] = {
#include "stg25/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg25/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg25/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg25/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[103] = {
#include "stg25/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg25/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[44] = {
#include "stg25/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg25/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg25/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg25/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[85] = {
#include "stg25/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg25/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[68] = {
#include "stg25/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg25/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg25/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg25/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[45] = {
#include "stg25/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg25/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[36] = {
#include "stg25/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg25/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg25/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg25/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[50] = {
#include "stg25/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg25/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[24] = {
#include "stg25/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg25/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg25/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg25/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[50] = {
#include "stg25/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg25/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[24] = {
#include "stg25/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg25/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg25/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg25/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[15] = {
#include "stg25/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg25/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[8] = {
#include "stg25/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg25/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg25/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg25/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[66] = {
#include "stg25/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg25/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[27] = {
#include "stg25/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg25/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg25/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg25/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[28] = {
#include "stg25/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg25/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[12] = {
#include "stg25/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg25/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg25/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg25/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[25] = {
#include "stg25/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg25/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[12] = {
#include "stg25/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg25/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg25/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg25/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[39] = {
#include "stg25/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg25/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[26] = {
#include "stg25/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg25/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg25/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg25/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[54] = {
#include "stg25/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg25/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[24] = {
#include "stg25/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg25/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg25/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg25/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[20] = {
#include "stg25/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg25/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[8] = {
#include "stg25/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg25/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg25/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg25/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[20] = {
#include "stg25/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg25/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[8] = {
#include "stg25/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg25/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg25/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg25/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[20] = {
#include "stg25/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg25/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[8] = {
#include "stg25/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg25/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg25/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg25/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[25] = {
#include "stg25/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg25/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[8] = {
#include "stg25/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg25/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg25/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg25/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[39] = {
#include "stg25/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg25/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[14] = {
#include "stg25/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg25/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg25/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg25/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[56] = {
#include "stg25/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg25/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[32] = {
#include "stg25/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg25/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg25/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_96_materials[1] = {
#include "stg25/terrain/collision_96_materials.inc"
};
static Sint16 collision_96_faces[25] = {
#include "stg25/terrain/collision_96_faces.inc"
};
static NJS_MESHSET collision_96_meshsets[1] = {
#include "stg25/terrain/collision_96_meshsets.inc"
};
static NJS_POINT3 collision_96_points[8] = {
#include "stg25/terrain/collision_96_points.inc"
};
static NJS_MODEL collision_96_model = {
#include "stg25/terrain/collision_96_model.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg25/terrain/collision_96.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg25/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[108] = {
#include "stg25/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg25/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[44] = {
#include "stg25/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg25/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg25/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg25/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[37] = {
#include "stg25/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg25/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[12] = {
#include "stg25/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg25/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg25/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg25/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[119] = {
#include "stg25/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg25/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[42] = {
#include "stg25/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg25/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg25/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg25/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[15] = {
#include "stg25/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg25/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[8] = {
#include "stg25/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg25/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg25/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg25/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[43] = {
#include "stg25/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg25/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[18] = {
#include "stg25/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg25/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg25/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_105_materials[1] = {
#include "stg25/terrain/collision_105_materials.inc"
};
static Sint16 collision_105_faces[28] = {
#include "stg25/terrain/collision_105_faces.inc"
};
static NJS_MESHSET collision_105_meshsets[1] = {
#include "stg25/terrain/collision_105_meshsets.inc"
};
static NJS_POINT3 collision_105_points[12] = {
#include "stg25/terrain/collision_105_points.inc"
};
static NJS_MODEL collision_105_model = {
#include "stg25/terrain/collision_105_model.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg25/terrain/collision_105.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg25/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[25] = {
#include "stg25/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg25/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[12] = {
#include "stg25/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg25/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg25/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg25/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[16] = {
#include "stg25/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg25/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[8] = {
#include "stg25/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg25/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg25/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg25/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[39] = {
#include "stg25/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg25/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[14] = {
#include "stg25/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg25/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg25/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg25/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[56] = {
#include "stg25/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg25/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[32] = {
#include "stg25/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg25/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg25/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg25/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[25] = {
#include "stg25/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg25/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[8] = {
#include "stg25/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg25/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg25/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg25/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[32] = {
#include "stg25/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg25/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[14] = {
#include "stg25/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg25/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg25/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg25/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[15] = {
#include "stg25/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg25/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[8] = {
#include "stg25/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg25/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg25/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg25/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[51] = {
#include "stg25/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg25/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[20] = {
#include "stg25/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg25/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg25/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg25/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[25] = {
#include "stg25/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg25/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[12] = {
#include "stg25/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg25/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg25/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg25/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[28] = {
#include "stg25/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg25/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[12] = {
#include "stg25/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg25/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg25/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg25/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[30] = {
#include "stg25/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg25/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[20] = {
#include "stg25/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg25/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg25/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg25/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[30] = {
#include "stg25/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg25/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[12] = {
#include "stg25/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg25/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg25/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg25/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[5] = {
#include "stg25/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg25/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[4] = {
#include "stg25/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg25/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg25/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_126_materials[1] = {
#include "stg25/terrain/collision_126_materials.inc"
};
static Sint16 collision_126_faces[125] = {
#include "stg25/terrain/collision_126_faces.inc"
};
static NJS_MESHSET collision_126_meshsets[1] = {
#include "stg25/terrain/collision_126_meshsets.inc"
};
static NJS_POINT3 collision_126_points[46] = {
#include "stg25/terrain/collision_126_points.inc"
};
static NJS_MODEL collision_126_model = {
#include "stg25/terrain/collision_126_model.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg25/terrain/collision_126.inc"
};
static NJS_MATERIAL collision_128_materials[1] = {
#include "stg25/terrain/collision_128_materials.inc"
};
static Sint16 collision_128_faces[5] = {
#include "stg25/terrain/collision_128_faces.inc"
};
static NJS_MESHSET collision_128_meshsets[1] = {
#include "stg25/terrain/collision_128_meshsets.inc"
};
static NJS_POINT3 collision_128_points[4] = {
#include "stg25/terrain/collision_128_points.inc"
};
static NJS_MODEL collision_128_model = {
#include "stg25/terrain/collision_128_model.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg25/terrain/collision_128.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg25/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[145] = {
#include "stg25/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg25/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[46] = {
#include "stg25/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg25/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg25/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg25/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[15] = {
#include "stg25/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg25/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[8] = {
#include "stg25/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg25/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg25/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg25/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[15] = {
#include "stg25/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg25/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[8] = {
#include "stg25/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg25/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg25/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_132_materials[1] = {
#include "stg25/terrain/collision_132_materials.inc"
};
static Sint16 collision_132_faces[15] = {
#include "stg25/terrain/collision_132_faces.inc"
};
static NJS_MESHSET collision_132_meshsets[1] = {
#include "stg25/terrain/collision_132_meshsets.inc"
};
static NJS_POINT3 collision_132_points[8] = {
#include "stg25/terrain/collision_132_points.inc"
};
static NJS_MODEL collision_132_model = {
#include "stg25/terrain/collision_132_model.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg25/terrain/collision_132.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg25/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[309] = {
#include "stg25/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg25/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[128] = {
#include "stg25/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg25/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg25/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg25/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[22] = {
#include "stg25/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg25/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[12] = {
#include "stg25/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg25/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg25/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg25/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[99] = {
#include "stg25/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg25/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[52] = {
#include "stg25/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg25/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg25/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg25/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[142] = {
#include "stg25/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg25/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[56] = {
#include "stg25/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg25/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg25/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg25/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[211] = {
#include "stg25/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg25/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[78] = {
#include "stg25/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg25/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg25/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg25/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[46] = {
#include "stg25/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg25/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[22] = {
#include "stg25/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg25/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg25/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg25/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[49] = {
#include "stg25/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg25/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[22] = {
#include "stg25/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg25/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg25/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg25/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[76] = {
#include "stg25/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg25/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[44] = {
#include "stg25/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg25/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg25/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg25/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[40] = {
#include "stg25/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg25/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[18] = {
#include "stg25/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg25/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg25/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg25/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[12] = {
#include "stg25/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg25/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[7] = {
#include "stg25/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg25/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg25/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg25/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[360] = {
#include "stg25/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg25/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[131] = {
#include "stg25/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg25/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg25/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg25/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[374] = {
#include "stg25/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg25/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[148] = {
#include "stg25/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg25/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg25/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg25/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[218] = {
#include "stg25/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg25/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[80] = {
#include "stg25/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg25/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg25/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg25/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[11] = {
#include "stg25/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg25/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[8] = {
#include "stg25/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg25/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg25/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg25/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[118] = {
#include "stg25/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg25/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[40] = {
#include "stg25/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg25/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg25/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg25/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[118] = {
#include "stg25/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg25/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[40] = {
#include "stg25/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg25/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg25/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg25/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[27] = {
#include "stg25/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg25/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[12] = {
#include "stg25/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg25/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg25/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_181_materials[1] = {
#include "stg25/terrain/collision_181_materials.inc"
};
static Sint16 collision_181_faces[32] = {
#include "stg25/terrain/collision_181_faces.inc"
};
static NJS_MESHSET collision_181_meshsets[1] = {
#include "stg25/terrain/collision_181_meshsets.inc"
};
static NJS_POINT3 collision_181_points[12] = {
#include "stg25/terrain/collision_181_points.inc"
};
static NJS_MODEL collision_181_model = {
#include "stg25/terrain/collision_181_model.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg25/terrain/collision_181.inc"
};
static NJS_MATERIAL collision_183_materials[1] = {
#include "stg25/terrain/collision_183_materials.inc"
};
static Sint16 collision_183_faces[32] = {
#include "stg25/terrain/collision_183_faces.inc"
};
static NJS_MESHSET collision_183_meshsets[1] = {
#include "stg25/terrain/collision_183_meshsets.inc"
};
static NJS_POINT3 collision_183_points[12] = {
#include "stg25/terrain/collision_183_points.inc"
};
static NJS_MODEL collision_183_model = {
#include "stg25/terrain/collision_183_model.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg25/terrain/collision_183.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg25/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[34] = {
#include "stg25/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg25/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[14] = {
#include "stg25/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg25/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg25/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg25/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[156] = {
#include "stg25/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg25/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[70] = {
#include "stg25/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg25/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg25/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg25/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[44] = {
#include "stg25/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg25/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[20] = {
#include "stg25/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg25/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg25/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_195_materials[1] = {
#include "stg25/terrain/collision_195_materials.inc"
};
static Sint16 collision_195_faces[45] = {
#include "stg25/terrain/collision_195_faces.inc"
};
static NJS_MESHSET collision_195_meshsets[1] = {
#include "stg25/terrain/collision_195_meshsets.inc"
};
static NJS_POINT3 collision_195_points[18] = {
#include "stg25/terrain/collision_195_points.inc"
};
static NJS_MODEL collision_195_model = {
#include "stg25/terrain/collision_195_model.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg25/terrain/collision_195.inc"
};
static NJS_MATERIAL collision_198_materials[1] = {
#include "stg25/terrain/collision_198_materials.inc"
};
static Sint16 collision_198_faces[32] = {
#include "stg25/terrain/collision_198_faces.inc"
};
static NJS_MESHSET collision_198_meshsets[1] = {
#include "stg25/terrain/collision_198_meshsets.inc"
};
static NJS_POINT3 collision_198_points[14] = {
#include "stg25/terrain/collision_198_points.inc"
};
static NJS_MODEL collision_198_model = {
#include "stg25/terrain/collision_198_model.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg25/terrain/collision_198.inc"
};
static NJS_MATERIAL collision_201_materials[1] = {
#include "stg25/terrain/collision_201_materials.inc"
};
static Sint16 collision_201_faces[29] = {
#include "stg25/terrain/collision_201_faces.inc"
};
static NJS_MESHSET collision_201_meshsets[1] = {
#include "stg25/terrain/collision_201_meshsets.inc"
};
static NJS_POINT3 collision_201_points[12] = {
#include "stg25/terrain/collision_201_points.inc"
};
static NJS_MODEL collision_201_model = {
#include "stg25/terrain/collision_201_model.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg25/terrain/collision_201.inc"
};
static NJS_MATERIAL collision_202_materials[1] = {
#include "stg25/terrain/collision_202_materials.inc"
};
static Sint16 collision_202_faces[32] = {
#include "stg25/terrain/collision_202_faces.inc"
};
static NJS_MESHSET collision_202_meshsets[1] = {
#include "stg25/terrain/collision_202_meshsets.inc"
};
static NJS_POINT3 collision_202_points[16] = {
#include "stg25/terrain/collision_202_points.inc"
};
static NJS_MODEL collision_202_model = {
#include "stg25/terrain/collision_202_model.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg25/terrain/collision_202.inc"
};
static NJS_MATERIAL collision_203_materials[1] = {
#include "stg25/terrain/collision_203_materials.inc"
};
static Sint16 collision_203_faces[33] = {
#include "stg25/terrain/collision_203_faces.inc"
};
static NJS_MESHSET collision_203_meshsets[1] = {
#include "stg25/terrain/collision_203_meshsets.inc"
};
static NJS_POINT3 collision_203_points[15] = {
#include "stg25/terrain/collision_203_points.inc"
};
static NJS_MODEL collision_203_model = {
#include "stg25/terrain/collision_203_model.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg25/terrain/collision_203.inc"
};
static NJS_MATERIAL collision_204_materials[1] = {
#include "stg25/terrain/collision_204_materials.inc"
};
static Sint16 collision_204_faces[12] = {
#include "stg25/terrain/collision_204_faces.inc"
};
static NJS_MESHSET collision_204_meshsets[1] = {
#include "stg25/terrain/collision_204_meshsets.inc"
};
static NJS_POINT3 collision_204_points[8] = {
#include "stg25/terrain/collision_204_points.inc"
};
static NJS_MODEL collision_204_model = {
#include "stg25/terrain/collision_204_model.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg25/terrain/collision_204.inc"
};
static NJS_MATERIAL collision_205_materials[1] = {
#include "stg25/terrain/collision_205_materials.inc"
};
static Sint16 collision_205_faces[12] = {
#include "stg25/terrain/collision_205_faces.inc"
};
static NJS_MESHSET collision_205_meshsets[1] = {
#include "stg25/terrain/collision_205_meshsets.inc"
};
static NJS_POINT3 collision_205_points[8] = {
#include "stg25/terrain/collision_205_points.inc"
};
static NJS_MODEL collision_205_model = {
#include "stg25/terrain/collision_205_model.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg25/terrain/collision_205.inc"
};
static NJS_MATERIAL collision_206_materials[1] = {
#include "stg25/terrain/collision_206_materials.inc"
};
static Sint16 collision_206_faces[18] = {
#include "stg25/terrain/collision_206_faces.inc"
};
static NJS_MESHSET collision_206_meshsets[1] = {
#include "stg25/terrain/collision_206_meshsets.inc"
};
static NJS_POINT3 collision_206_points[12] = {
#include "stg25/terrain/collision_206_points.inc"
};
static NJS_MODEL collision_206_model = {
#include "stg25/terrain/collision_206_model.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg25/terrain/collision_206.inc"
};
static NJS_MATERIAL collision_212_materials[1] = {
#include "stg25/terrain/collision_212_materials.inc"
};
static Sint16 collision_212_faces[96] = {
#include "stg25/terrain/collision_212_faces.inc"
};
static NJS_MESHSET collision_212_meshsets[1] = {
#include "stg25/terrain/collision_212_meshsets.inc"
};
static NJS_POINT3 collision_212_points[56] = {
#include "stg25/terrain/collision_212_points.inc"
};
static NJS_MODEL collision_212_model = {
#include "stg25/terrain/collision_212_model.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg25/terrain/collision_212.inc"
};
static NJS_MATERIAL collision_215_materials[1] = {
#include "stg25/terrain/collision_215_materials.inc"
};
static Sint16 collision_215_faces[18] = {
#include "stg25/terrain/collision_215_faces.inc"
};
static NJS_MESHSET collision_215_meshsets[1] = {
#include "stg25/terrain/collision_215_meshsets.inc"
};
static NJS_POINT3 collision_215_points[12] = {
#include "stg25/terrain/collision_215_points.inc"
};
static NJS_MODEL collision_215_model = {
#include "stg25/terrain/collision_215_model.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg25/terrain/collision_215.inc"
};
static NJS_MATERIAL collision_221_materials[1] = {
#include "stg25/terrain/collision_221_materials.inc"
};
static Sint16 collision_221_faces[47] = {
#include "stg25/terrain/collision_221_faces.inc"
};
static NJS_MESHSET collision_221_meshsets[1] = {
#include "stg25/terrain/collision_221_meshsets.inc"
};
static NJS_POINT3 collision_221_points[25] = {
#include "stg25/terrain/collision_221_points.inc"
};
static NJS_MODEL collision_221_model = {
#include "stg25/terrain/collision_221_model.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg25/terrain/collision_221.inc"
};
static NJS_MATERIAL collision_222_materials[1] = {
#include "stg25/terrain/collision_222_materials.inc"
};
static Sint16 collision_222_faces[32] = {
#include "stg25/terrain/collision_222_faces.inc"
};
static NJS_MESHSET collision_222_meshsets[1] = {
#include "stg25/terrain/collision_222_meshsets.inc"
};
static NJS_POINT3 collision_222_points[12] = {
#include "stg25/terrain/collision_222_points.inc"
};
static NJS_MODEL collision_222_model = {
#include "stg25/terrain/collision_222_model.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg25/terrain/collision_222.inc"
};
static NJS_MATERIAL collision_224_materials[1] = {
#include "stg25/terrain/collision_224_materials.inc"
};
static Sint16 collision_224_faces[32] = {
#include "stg25/terrain/collision_224_faces.inc"
};
static NJS_MESHSET collision_224_meshsets[1] = {
#include "stg25/terrain/collision_224_meshsets.inc"
};
static NJS_POINT3 collision_224_points[12] = {
#include "stg25/terrain/collision_224_points.inc"
};
static NJS_MODEL collision_224_model = {
#include "stg25/terrain/collision_224_model.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg25/terrain/collision_224.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg25/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[94] = {
#include "stg25/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg25/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[35] = {
#include "stg25/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg25/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg25/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_226_materials[1] = {
#include "stg25/terrain/collision_226_materials.inc"
};
static Sint16 collision_226_faces[27] = {
#include "stg25/terrain/collision_226_faces.inc"
};
static NJS_MESHSET collision_226_meshsets[1] = {
#include "stg25/terrain/collision_226_meshsets.inc"
};
static NJS_POINT3 collision_226_points[11] = {
#include "stg25/terrain/collision_226_points.inc"
};
static NJS_MODEL collision_226_model = {
#include "stg25/terrain/collision_226_model.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg25/terrain/collision_226.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg25/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[27] = {
#include "stg25/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg25/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[11] = {
#include "stg25/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg25/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg25/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_228_materials[1] = {
#include "stg25/terrain/collision_228_materials.inc"
};
static Sint16 collision_228_faces[9] = {
#include "stg25/terrain/collision_228_faces.inc"
};
static NJS_MESHSET collision_228_meshsets[1] = {
#include "stg25/terrain/collision_228_meshsets.inc"
};
static NJS_POINT3 collision_228_points[8] = {
#include "stg25/terrain/collision_228_points.inc"
};
static NJS_MODEL collision_228_model = {
#include "stg25/terrain/collision_228_model.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg25/terrain/collision_228.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg25/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[45] = {
#include "stg25/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg25/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[24] = {
#include "stg25/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg25/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg25/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg25/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[32] = {
#include "stg25/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg25/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[12] = {
#include "stg25/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg25/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg25/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_235_materials[1] = {
#include "stg25/terrain/collision_235_materials.inc"
};
static Sint16 collision_235_faces[32] = {
#include "stg25/terrain/collision_235_faces.inc"
};
static NJS_MESHSET collision_235_meshsets[1] = {
#include "stg25/terrain/collision_235_meshsets.inc"
};
static NJS_POINT3 collision_235_points[12] = {
#include "stg25/terrain/collision_235_points.inc"
};
static NJS_MODEL collision_235_model = {
#include "stg25/terrain/collision_235_model.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg25/terrain/collision_235.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg25/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[141] = {
#include "stg25/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg25/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[46] = {
#include "stg25/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg25/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg25/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_237_materials[1] = {
#include "stg25/terrain/collision_237_materials.inc"
};
static Sint16 collision_237_faces[28] = {
#include "stg25/terrain/collision_237_faces.inc"
};
static NJS_MESHSET collision_237_meshsets[1] = {
#include "stg25/terrain/collision_237_meshsets.inc"
};
static NJS_POINT3 collision_237_points[11] = {
#include "stg25/terrain/collision_237_points.inc"
};
static NJS_MODEL collision_237_model = {
#include "stg25/terrain/collision_237_model.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg25/terrain/collision_237.inc"
};
static NJS_MATERIAL collision_238_materials[1] = {
#include "stg25/terrain/collision_238_materials.inc"
};
static Sint16 collision_238_faces[28] = {
#include "stg25/terrain/collision_238_faces.inc"
};
static NJS_MESHSET collision_238_meshsets[1] = {
#include "stg25/terrain/collision_238_meshsets.inc"
};
static NJS_POINT3 collision_238_points[11] = {
#include "stg25/terrain/collision_238_points.inc"
};
static NJS_MODEL collision_238_model = {
#include "stg25/terrain/collision_238_model.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg25/terrain/collision_238.inc"
};
static NJS_MATERIAL collision_239_materials[1] = {
#include "stg25/terrain/collision_239_materials.inc"
};
static Sint16 collision_239_faces[54] = {
#include "stg25/terrain/collision_239_faces.inc"
};
static NJS_MESHSET collision_239_meshsets[1] = {
#include "stg25/terrain/collision_239_meshsets.inc"
};
static NJS_POINT3 collision_239_points[23] = {
#include "stg25/terrain/collision_239_points.inc"
};
static NJS_MODEL collision_239_model = {
#include "stg25/terrain/collision_239_model.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg25/terrain/collision_239.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg25/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[34] = {
#include "stg25/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg25/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[19] = {
#include "stg25/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg25/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg25/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_241_materials[1] = {
#include "stg25/terrain/collision_241_materials.inc"
};
static Sint16 collision_241_faces[25] = {
#include "stg25/terrain/collision_241_faces.inc"
};
static NJS_MESHSET collision_241_meshsets[1] = {
#include "stg25/terrain/collision_241_meshsets.inc"
};
static NJS_POINT3 collision_241_points[12] = {
#include "stg25/terrain/collision_241_points.inc"
};
static NJS_MODEL collision_241_model = {
#include "stg25/terrain/collision_241_model.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg25/terrain/collision_241.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg25/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[31] = {
#include "stg25/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg25/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[12] = {
#include "stg25/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg25/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg25/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_243_materials[1] = {
#include "stg25/terrain/collision_243_materials.inc"
};
static Sint16 collision_243_faces[49] = {
#include "stg25/terrain/collision_243_faces.inc"
};
static NJS_MESHSET collision_243_meshsets[1] = {
#include "stg25/terrain/collision_243_meshsets.inc"
};
static NJS_POINT3 collision_243_points[20] = {
#include "stg25/terrain/collision_243_points.inc"
};
static NJS_MODEL collision_243_model = {
#include "stg25/terrain/collision_243_model.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg25/terrain/collision_243.inc"
};
static NJS_MATERIAL collision_244_materials[1] = {
#include "stg25/terrain/collision_244_materials.inc"
};
static Sint16 collision_244_faces[23] = {
#include "stg25/terrain/collision_244_faces.inc"
};
static NJS_MESHSET collision_244_meshsets[1] = {
#include "stg25/terrain/collision_244_meshsets.inc"
};
static NJS_POINT3 collision_244_points[16] = {
#include "stg25/terrain/collision_244_points.inc"
};
static NJS_MODEL collision_244_model = {
#include "stg25/terrain/collision_244_model.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg25/terrain/collision_244.inc"
};
static NJS_MATERIAL collision_246_materials[1] = {
#include "stg25/terrain/collision_246_materials.inc"
};
static Sint16 collision_246_faces[7] = {
#include "stg25/terrain/collision_246_faces.inc"
};
static NJS_MESHSET collision_246_meshsets[1] = {
#include "stg25/terrain/collision_246_meshsets.inc"
};
static NJS_POINT3 collision_246_points[6] = {
#include "stg25/terrain/collision_246_points.inc"
};
static NJS_MODEL collision_246_model = {
#include "stg25/terrain/collision_246_model.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg25/terrain/collision_246.inc"
};
static NJS_MATERIAL collision_247_materials[1] = {
#include "stg25/terrain/collision_247_materials.inc"
};
static Sint16 collision_247_faces[7] = {
#include "stg25/terrain/collision_247_faces.inc"
};
static NJS_MESHSET collision_247_meshsets[1] = {
#include "stg25/terrain/collision_247_meshsets.inc"
};
static NJS_POINT3 collision_247_points[6] = {
#include "stg25/terrain/collision_247_points.inc"
};
static NJS_MODEL collision_247_model = {
#include "stg25/terrain/collision_247_model.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg25/terrain/collision_247.inc"
};
static NJS_MATERIAL collision_248_materials[1] = {
#include "stg25/terrain/collision_248_materials.inc"
};
static Sint16 collision_248_faces[94] = {
#include "stg25/terrain/collision_248_faces.inc"
};
static NJS_MESHSET collision_248_meshsets[1] = {
#include "stg25/terrain/collision_248_meshsets.inc"
};
static NJS_POINT3 collision_248_points[46] = {
#include "stg25/terrain/collision_248_points.inc"
};
static NJS_MODEL collision_248_model = {
#include "stg25/terrain/collision_248_model.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg25/terrain/collision_248.inc"
};
static NJS_MATERIAL collision_249_materials[1] = {
#include "stg25/terrain/collision_249_materials.inc"
};
static Sint16 collision_249_faces[39] = {
#include "stg25/terrain/collision_249_faces.inc"
};
static NJS_MESHSET collision_249_meshsets[1] = {
#include "stg25/terrain/collision_249_meshsets.inc"
};
static NJS_POINT3 collision_249_points[17] = {
#include "stg25/terrain/collision_249_points.inc"
};
static NJS_MODEL collision_249_model = {
#include "stg25/terrain/collision_249_model.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg25/terrain/collision_249.inc"
};
static NJS_MATERIAL collision_250_materials[1] = {
#include "stg25/terrain/collision_250_materials.inc"
};
static Sint16 collision_250_faces[5] = {
#include "stg25/terrain/collision_250_faces.inc"
};
static NJS_MESHSET collision_250_meshsets[1] = {
#include "stg25/terrain/collision_250_meshsets.inc"
};
static NJS_POINT3 collision_250_points[4] = {
#include "stg25/terrain/collision_250_points.inc"
};
static NJS_MODEL collision_250_model = {
#include "stg25/terrain/collision_250_model.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg25/terrain/collision_250.inc"
};
static NJS_MATERIAL collision_251_materials[1] = {
#include "stg25/terrain/collision_251_materials.inc"
};
static Sint16 collision_251_faces[5] = {
#include "stg25/terrain/collision_251_faces.inc"
};
static NJS_MESHSET collision_251_meshsets[1] = {
#include "stg25/terrain/collision_251_meshsets.inc"
};
static NJS_POINT3 collision_251_points[4] = {
#include "stg25/terrain/collision_251_points.inc"
};
static NJS_MODEL collision_251_model = {
#include "stg25/terrain/collision_251_model.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg25/terrain/collision_251.inc"
};
static Sint16 draw_263_polygons[45] = {
#include "stg25/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[39] = {
#include "stg25/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg25/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg25/terrain/draw_263.inc"
};
static Sint16 draw_259_polygons[103] = {
#include "stg25/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[75] = {
#include "stg25/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg25/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg25/terrain/draw_262.inc"
};
static Sint16 draw_260_polygons[779] = {
#include "stg25/terrain/draw_260_polygons.inc"
};
static Sint32 draw_260_vertices[391] = {
#include "stg25/terrain/draw_260_vertices.inc"
};
static NJS_CNK_MODEL draw_260_model = {
#include "stg25/terrain/draw_260_model.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg25/terrain/draw_260.inc"
};
static Sint16 draw_258_polygons[783] = {
#include "stg25/terrain/draw_258_polygons.inc"
};
static Sint32 draw_258_vertices[671] = {
#include "stg25/terrain/draw_258_vertices.inc"
};
static NJS_CNK_MODEL draw_258_model = {
#include "stg25/terrain/draw_258_model.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg25/terrain/draw_258.inc"
};
static Sint16 draw_257_polygons[675] = {
#include "stg25/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[331] = {
#include "stg25/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg25/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg25/terrain/draw_257.inc"
};
static Sint16 draw_256_polygons[663] = {
#include "stg25/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[315] = {
#include "stg25/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg25/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg25/terrain/draw_256.inc"
};
static Sint16 draw_255_polygons[783] = {
#include "stg25/terrain/draw_255_polygons.inc"
};
static Sint32 draw_255_vertices[659] = {
#include "stg25/terrain/draw_255_vertices.inc"
};
static NJS_CNK_MODEL draw_255_model = {
#include "stg25/terrain/draw_255_model.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg25/terrain/draw_255.inc"
};
static Sint16 draw_253_polygons[927] = {
#include "stg25/terrain/draw_253_polygons.inc"
};
static Sint32 draw_253_vertices[851] = {
#include "stg25/terrain/draw_253_vertices.inc"
};
static NJS_CNK_MODEL draw_253_model = {
#include "stg25/terrain/draw_253_model.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg25/terrain/draw_254.inc"
};
static Sint16 draw_252_polygons[519] = {
#include "stg25/terrain/draw_252_polygons.inc"
};
static Sint32 draw_252_vertices[527] = {
#include "stg25/terrain/draw_252_vertices.inc"
};
static NJS_CNK_MODEL draw_252_model = {
#include "stg25/terrain/draw_252_model.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg25/terrain/draw_252.inc"
};
static Sint16 draw_251_polygons[265] = {
#include "stg25/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[195] = {
#include "stg25/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg25/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg25/terrain/draw_251.inc"
};
static Sint16 draw_250_polygons[195] = {
#include "stg25/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[219] = {
#include "stg25/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg25/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg25/terrain/draw_250.inc"
};
static Sint16 draw_249_polygons[405] = {
#include "stg25/terrain/draw_249_polygons.inc"
};
static Sint32 draw_249_vertices[227] = {
#include "stg25/terrain/draw_249_vertices.inc"
};
static NJS_CNK_MODEL draw_249_model = {
#include "stg25/terrain/draw_249_model.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg25/terrain/draw_249.inc"
};
static Sint16 draw_248_polygons[249] = {
#include "stg25/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[183] = {
#include "stg25/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg25/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg25/terrain/draw_248.inc"
};
static Sint16 draw_247_polygons[71] = {
#include "stg25/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[39] = {
#include "stg25/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg25/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg25/terrain/draw_247.inc"
};
static Sint16 draw_246_polygons[289] = {
#include "stg25/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[131] = {
#include "stg25/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg25/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg25/terrain/draw_246.inc"
};
static Sint16 draw_245_polygons[67] = {
#include "stg25/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[55] = {
#include "stg25/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg25/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg25/terrain/draw_245.inc"
};
static Sint16 draw_244_polygons[329] = {
#include "stg25/terrain/draw_244_polygons.inc"
};
static Sint32 draw_244_vertices[231] = {
#include "stg25/terrain/draw_244_vertices.inc"
};
static NJS_CNK_MODEL draw_244_model = {
#include "stg25/terrain/draw_244_model.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg25/terrain/draw_244.inc"
};
static Sint16 draw_243_polygons[83] = {
#include "stg25/terrain/draw_243_polygons.inc"
};
static Sint32 draw_243_vertices[67] = {
#include "stg25/terrain/draw_243_vertices.inc"
};
static NJS_CNK_MODEL draw_243_model = {
#include "stg25/terrain/draw_243_model.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg25/terrain/draw_243.inc"
};
static Sint16 draw_242_polygons[115] = {
#include "stg25/terrain/draw_242_polygons.inc"
};
static Sint32 draw_242_vertices[83] = {
#include "stg25/terrain/draw_242_vertices.inc"
};
static NJS_CNK_MODEL draw_242_model = {
#include "stg25/terrain/draw_242_model.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg25/terrain/draw_242.inc"
};
static Sint16 draw_240_polygons[787] = {
#include "stg25/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[403] = {
#include "stg25/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg25/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg25/terrain/draw_241.inc"
};
static Sint16 draw_236_polygons[65] = {
#include "stg25/terrain/draw_236_polygons.inc"
};
static Sint32 draw_236_vertices[51] = {
#include "stg25/terrain/draw_236_vertices.inc"
};
static NJS_CNK_MODEL draw_236_model = {
#include "stg25/terrain/draw_236_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg25/terrain/draw_239.inc"
};
static Sint16 draw_234_polygons[217] = {
#include "stg25/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[143] = {
#include "stg25/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg25/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg25/terrain/draw_235.inc"
};
static Sint16 draw_230_polygons[121] = {
#include "stg25/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[71] = {
#include "stg25/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg25/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg25/terrain/draw_233.inc"
};
static Sint16 draw_227_polygons[89] = {
#include "stg25/terrain/draw_227_polygons.inc"
};
static Sint32 draw_227_vertices[75] = {
#include "stg25/terrain/draw_227_vertices.inc"
};
static NJS_CNK_MODEL draw_227_model = {
#include "stg25/terrain/draw_227_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg25/terrain/draw_229.inc"
};
static Sint16 draw_219_polygons[89] = {
#include "stg25/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[59] = {
#include "stg25/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg25/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg25/terrain/draw_226.inc"
};
static Sint16 draw_210_polygons[57] = {
#include "stg25/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[43] = {
#include "stg25/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg25/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg25/terrain/draw_218.inc"
};
static Sint16 draw_208_polygons[109] = {
#include "stg25/terrain/draw_208_polygons.inc"
};
static Sint32 draw_208_vertices[67] = {
#include "stg25/terrain/draw_208_vertices.inc"
};
static NJS_CNK_MODEL draw_208_model = {
#include "stg25/terrain/draw_208_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg25/terrain/draw_209.inc"
};
static Sint16 draw_204_polygons[65] = {
#include "stg25/terrain/draw_204_polygons.inc"
};
static Sint32 draw_204_vertices[51] = {
#include "stg25/terrain/draw_204_vertices.inc"
};
static NJS_CNK_MODEL draw_204_model = {
#include "stg25/terrain/draw_204_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg25/terrain/draw_207.inc"
};
static Sint16 draw_202_polygons[45] = {
#include "stg25/terrain/draw_202_polygons.inc"
};
static Sint32 draw_202_vertices[43] = {
#include "stg25/terrain/draw_202_vertices.inc"
};
static NJS_CNK_MODEL draw_202_model = {
#include "stg25/terrain/draw_202_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg25/terrain/draw_203.inc"
};
static Sint16 draw_200_polygons[343] = {
#include "stg25/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[163] = {
#include "stg25/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg25/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg25/terrain/draw_201.inc"
};
static Sint16 draw_198_polygons[339] = {
#include "stg25/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[195] = {
#include "stg25/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg25/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg25/terrain/draw_198.inc"
};
static Sint16 draw_196_polygons[331] = {
#include "stg25/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[163] = {
#include "stg25/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg25/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg25/terrain/draw_196.inc"
};
static Sint16 draw_195_polygons[255] = {
#include "stg25/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[107] = {
#include "stg25/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg25/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg25/terrain/draw_195.inc"
};
static Sint16 draw_194_polygons[307] = {
#include "stg25/terrain/draw_194_polygons.inc"
};
static Sint32 draw_194_vertices[171] = {
#include "stg25/terrain/draw_194_vertices.inc"
};
static NJS_CNK_MODEL draw_194_model = {
#include "stg25/terrain/draw_194_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg25/terrain/draw_194.inc"
};
static Sint16 draw_192_polygons[845] = {
#include "stg25/terrain/draw_192_polygons.inc"
};
static Sint32 draw_192_vertices[699] = {
#include "stg25/terrain/draw_192_vertices.inc"
};
static NJS_CNK_MODEL draw_192_model = {
#include "stg25/terrain/draw_192_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg25/terrain/draw_193.inc"
};
static Sint16 draw_190_polygons[189] = {
#include "stg25/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[195] = {
#include "stg25/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg25/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg25/terrain/draw_191.inc"
};
static Sint16 draw_188_polygons[165] = {
#include "stg25/terrain/draw_188_polygons.inc"
};
static Sint32 draw_188_vertices[195] = {
#include "stg25/terrain/draw_188_vertices.inc"
};
static NJS_CNK_MODEL draw_188_model = {
#include "stg25/terrain/draw_188_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg25/terrain/draw_189.inc"
};
static Sint16 draw_187_polygons[555] = {
#include "stg25/terrain/draw_187_polygons.inc"
};
static Sint32 draw_187_vertices[239] = {
#include "stg25/terrain/draw_187_vertices.inc"
};
static NJS_CNK_MODEL draw_187_model = {
#include "stg25/terrain/draw_187_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg25/terrain/draw_187.inc"
};
static Sint16 draw_183_polygons[57] = {
#include "stg25/terrain/draw_183_polygons.inc"
};
static Sint32 draw_183_vertices[43] = {
#include "stg25/terrain/draw_183_vertices.inc"
};
static NJS_CNK_MODEL draw_183_model = {
#include "stg25/terrain/draw_183_model.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg25/terrain/draw_186.inc"
};
static Sint16 draw_182_polygons[575] = {
#include "stg25/terrain/draw_182_polygons.inc"
};
static Sint32 draw_182_vertices[247] = {
#include "stg25/terrain/draw_182_vertices.inc"
};
static NJS_CNK_MODEL draw_182_model = {
#include "stg25/terrain/draw_182_model.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg25/terrain/draw_182.inc"
};
static Sint16 draw_178_polygons[57] = {
#include "stg25/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[43] = {
#include "stg25/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg25/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg25/terrain/draw_181.inc"
};
static Sint16 draw_170_polygons[61] = {
#include "stg25/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[43] = {
#include "stg25/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg25/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg25/terrain/draw_177.inc"
};
static Sint16 draw_169_polygons[63] = {
#include "stg25/terrain/draw_169_polygons.inc"
};
static Sint32 draw_169_vertices[35] = {
#include "stg25/terrain/draw_169_vertices.inc"
};
static NJS_CNK_MODEL draw_169_model = {
#include "stg25/terrain/draw_169_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg25/terrain/draw_169.inc"
};
static Sint16 draw_168_polygons[81] = {
#include "stg25/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[59] = {
#include "stg25/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg25/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg25/terrain/draw_168.inc"
};
static Sint16 draw_167_polygons[1341] = {
#include "stg25/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[1015] = {
#include "stg25/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg25/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg25/terrain/draw_167.inc"
};
static Sint16 draw_163_polygons[51] = {
#include "stg25/terrain/draw_163_polygons.inc"
};
static Sint32 draw_163_vertices[35] = {
#include "stg25/terrain/draw_163_vertices.inc"
};
static NJS_CNK_MODEL draw_163_model = {
#include "stg25/terrain/draw_163_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg25/terrain/draw_166.inc"
};
static Sint16 draw_160_polygons[141] = {
#include "stg25/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[123] = {
#include "stg25/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg25/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg25/terrain/draw_162.inc"
};
static Sint16 draw_159_polygons[101] = {
#include "stg25/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[67] = {
#include "stg25/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg25/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg25/terrain/draw_159.inc"
};
static Sint16 draw_151_polygons[63] = {
#include "stg25/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[59] = {
#include "stg25/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg25/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg25/terrain/draw_158.inc"
};
static Sint16 draw_149_polygons[51] = {
#include "stg25/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[51] = {
#include "stg25/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg25/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg25/terrain/draw_150.inc"
};
static Sint16 draw_148_polygons[343] = {
#include "stg25/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[159] = {
#include "stg25/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg25/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg25/terrain/draw_148.inc"
};
static Sint16 draw_147_polygons[1195] = {
#include "stg25/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[571] = {
#include "stg25/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg25/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg25/terrain/draw_147.inc"
};
static Sint16 draw_145_polygons[75] = {
#include "stg25/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[51] = {
#include "stg25/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg25/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg25/terrain/draw_146.inc"
};
static Sint16 draw_143_polygons[57] = {
#include "stg25/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[51] = {
#include "stg25/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg25/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg25/terrain/draw_144.inc"
};
static Sint16 draw_142_polygons[1195] = {
#include "stg25/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[567] = {
#include "stg25/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg25/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg25/terrain/draw_142.inc"
};
static Sint16 draw_141_polygons[85] = {
#include "stg25/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[63] = {
#include "stg25/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg25/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg25/terrain/draw_141.inc"
};
static Sint16 draw_140_polygons[81] = {
#include "stg25/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[59] = {
#include "stg25/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg25/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg25/terrain/draw_140.inc"
};
static Sint16 draw_139_polygons[81] = {
#include "stg25/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[59] = {
#include "stg25/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg25/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg25/terrain/draw_139.inc"
};
static Sint16 draw_138_polygons[209] = {
#include "stg25/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[151] = {
#include "stg25/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg25/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg25/terrain/draw_138.inc"
};
static Sint16 draw_137_polygons[219] = {
#include "stg25/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[131] = {
#include "stg25/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg25/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg25/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[219] = {
#include "stg25/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[131] = {
#include "stg25/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg25/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg25/terrain/draw_136.inc"
};
static Sint16 draw_132_polygons[269] = {
#include "stg25/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[143] = {
#include "stg25/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg25/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg25/terrain/draw_135.inc"
};
static Sint16 draw_131_polygons[263] = {
#include "stg25/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[147] = {
#include "stg25/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg25/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg25/terrain/draw_131.inc"
};
static Sint16 draw_128_polygons[269] = {
#include "stg25/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[147] = {
#include "stg25/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg25/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg25/terrain/draw_130.inc"
};
static Sint16 draw_127_polygons[173] = {
#include "stg25/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[103] = {
#include "stg25/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg25/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg25/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[235] = {
#include "stg25/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[227] = {
#include "stg25/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg25/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg25/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[255] = {
#include "stg25/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[159] = {
#include "stg25/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg25/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg25/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[229] = {
#include "stg25/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[219] = {
#include "stg25/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg25/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg25/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[261] = {
#include "stg25/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[159] = {
#include "stg25/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg25/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg25/terrain/draw_123.inc"
};
static Sint16 draw_122_polygons[229] = {
#include "stg25/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[223] = {
#include "stg25/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg25/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg25/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[83] = {
#include "stg25/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[55] = {
#include "stg25/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg25/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg25/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[229] = {
#include "stg25/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[223] = {
#include "stg25/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg25/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg25/terrain/draw_120.inc"
};
static Sint16 draw_119_polygons[121] = {
#include "stg25/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[67] = {
#include "stg25/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg25/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg25/terrain/draw_119.inc"
};
static Sint16 draw_118_polygons[1003] = {
#include "stg25/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[827] = {
#include "stg25/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg25/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg25/terrain/draw_118.inc"
};
static Sint16 draw_117_polygons[369] = {
#include "stg25/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[279] = {
#include "stg25/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg25/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg25/terrain/draw_117.inc"
};
static Sint16 draw_116_polygons[369] = {
#include "stg25/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[279] = {
#include "stg25/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg25/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg25/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[361] = {
#include "stg25/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[279] = {
#include "stg25/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg25/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg25/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[375] = {
#include "stg25/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[279] = {
#include "stg25/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg25/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg25/terrain/draw_114.inc"
};
static Sint16 draw_113_polygons[95] = {
#include "stg25/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[71] = {
#include "stg25/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg25/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg25/terrain/draw_113.inc"
};
static Sint16 draw_112_polygons[155] = {
#include "stg25/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[143] = {
#include "stg25/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg25/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg25/terrain/draw_112.inc"
};
static Sint16 draw_111_polygons[93] = {
#include "stg25/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[83] = {
#include "stg25/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg25/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg25/terrain/draw_111.inc"
};
static Sint16 draw_110_polygons[165] = {
#include "stg25/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[147] = {
#include "stg25/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg25/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg25/terrain/draw_110.inc"
};
static Sint16 draw_109_polygons[385] = {
#include "stg25/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[335] = {
#include "stg25/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg25/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg25/terrain/draw_109.inc"
};
static Sint16 draw_108_polygons[141] = {
#include "stg25/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[83] = {
#include "stg25/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg25/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg25/terrain/draw_108.inc"
};
static Sint16 draw_107_polygons[2789] = {
#include "stg25/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[1539] = {
#include "stg25/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg25/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg25/terrain/draw_107.inc"
};
static Sint16 draw_103_polygons[1013] = {
#include "stg25/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[599] = {
#include "stg25/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg25/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg25/terrain/draw_106.inc"
};
static Sint16 draw_102_polygons[337] = {
#include "stg25/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[235] = {
#include "stg25/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg25/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg25/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[57] = {
#include "stg25/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[39] = {
#include "stg25/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg25/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg25/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[327] = {
#include "stg25/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[323] = {
#include "stg25/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg25/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg25/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[147] = {
#include "stg25/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[99] = {
#include "stg25/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg25/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg25/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[147] = {
#include "stg25/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[99] = {
#include "stg25/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg25/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg25/terrain/draw_98.inc"
};
static Sint16 draw_97_polygons[169] = {
#include "stg25/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[99] = {
#include "stg25/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg25/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg25/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[147] = {
#include "stg25/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[99] = {
#include "stg25/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg25/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg25/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[663] = {
#include "stg25/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[411] = {
#include "stg25/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg25/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg25/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[247] = {
#include "stg25/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[203] = {
#include "stg25/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg25/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg25/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[395] = {
#include "stg25/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[323] = {
#include "stg25/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg25/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg25/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[409] = {
#include "stg25/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[371] = {
#include "stg25/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg25/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg25/terrain/draw_92.inc"
};
static Sint16 draw_90_polygons[341] = {
#include "stg25/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[147] = {
#include "stg25/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg25/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg25/terrain/draw_91.inc"
};
static Sint16 draw_88_polygons[307] = {
#include "stg25/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[171] = {
#include "stg25/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg25/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg25/terrain/draw_89.inc"
};
static Sint16 draw_86_polygons[263] = {
#include "stg25/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[107] = {
#include "stg25/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg25/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg25/terrain/draw_87.inc"
};
static Sint16 draw_85_polygons[317] = {
#include "stg25/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[135] = {
#include "stg25/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg25/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg25/terrain/draw_85.inc"
};
static Sint16 draw_84_polygons[181] = {
#include "stg25/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[127] = {
#include "stg25/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg25/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg25/terrain/draw_84.inc"
};
static Sint16 draw_83_polygons[471] = {
#include "stg25/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[279] = {
#include "stg25/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg25/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg25/terrain/draw_83.inc"
};
static Sint16 draw_82_polygons[559] = {
#include "stg25/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[299] = {
#include "stg25/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg25/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg25/terrain/draw_82.inc"
};
static Sint16 draw_81_polygons[153] = {
#include "stg25/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[83] = {
#include "stg25/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg25/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg25/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[83] = {
#include "stg25/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[75] = {
#include "stg25/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg25/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg25/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[117] = {
#include "stg25/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[99] = {
#include "stg25/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg25/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg25/terrain/draw_79.inc"
};
static Sint16 draw_78_polygons[203] = {
#include "stg25/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[99] = {
#include "stg25/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg25/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg25/terrain/draw_78.inc"
};
static Sint16 draw_77_polygons[861] = {
#include "stg25/terrain/draw_77_polygons.inc"
};
static Sint32 draw_77_vertices[479] = {
#include "stg25/terrain/draw_77_vertices.inc"
};
static NJS_CNK_MODEL draw_77_model = {
#include "stg25/terrain/draw_77_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg25/terrain/draw_77.inc"
};
static Sint16 draw_76_polygons[197] = {
#include "stg25/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[107] = {
#include "stg25/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg25/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg25/terrain/draw_76.inc"
};
static Sint16 draw_75_polygons[135] = {
#include "stg25/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[75] = {
#include "stg25/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg25/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg25/terrain/draw_75.inc"
};
static Sint16 draw_74_polygons[133] = {
#include "stg25/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[71] = {
#include "stg25/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg25/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg25/terrain/draw_74.inc"
};
static Sint16 draw_73_polygons[181] = {
#include "stg25/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[99] = {
#include "stg25/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg25/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg25/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[161] = {
#include "stg25/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[131] = {
#include "stg25/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg25/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg25/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[203] = {
#include "stg25/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[99] = {
#include "stg25/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg25/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg25/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[181] = {
#include "stg25/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[99] = {
#include "stg25/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg25/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg25/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[245] = {
#include "stg25/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[179] = {
#include "stg25/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg25/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg25/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[245] = {
#include "stg25/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[179] = {
#include "stg25/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg25/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg25/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[485] = {
#include "stg25/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[367] = {
#include "stg25/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg25/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg25/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[359] = {
#include "stg25/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[199] = {
#include "stg25/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg25/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg25/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[147] = {
#include "stg25/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[99] = {
#include "stg25/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg25/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg25/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[169] = {
#include "stg25/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[99] = {
#include "stg25/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg25/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg25/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[147] = {
#include "stg25/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[99] = {
#include "stg25/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg25/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg25/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[769] = {
#include "stg25/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[475] = {
#include "stg25/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg25/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg25/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[195] = {
#include "stg25/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[139] = {
#include "stg25/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg25/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg25/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[395] = {
#include "stg25/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[315] = {
#include "stg25/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg25/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg25/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[423] = {
#include "stg25/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[379] = {
#include "stg25/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg25/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg25/terrain/draw_59.inc"
};
static Sint16 draw_57_polygons[255] = {
#include "stg25/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[91] = {
#include "stg25/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg25/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg25/terrain/draw_58.inc"
};
static Sint16 draw_56_polygons[521] = {
#include "stg25/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[291] = {
#include "stg25/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg25/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg25/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[153] = {
#include "stg25/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[83] = {
#include "stg25/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg25/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg25/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[83] = {
#include "stg25/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[67] = {
#include "stg25/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg25/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg25/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[103] = {
#include "stg25/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[91] = {
#include "stg25/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg25/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg25/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[189] = {
#include "stg25/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[99] = {
#include "stg25/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg25/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg25/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[869] = {
#include "stg25/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[491] = {
#include "stg25/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg25/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg25/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[309] = {
#include "stg25/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[167] = {
#include "stg25/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg25/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg25/terrain/draw_50.inc"
};
static Sint16 draw_49_polygons[133] = {
#include "stg25/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[71] = {
#include "stg25/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg25/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg25/terrain/draw_49.inc"
};
static Sint16 draw_48_polygons[189] = {
#include "stg25/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[103] = {
#include "stg25/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg25/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg25/terrain/draw_48.inc"
};
static Sint16 draw_47_polygons[161] = {
#include "stg25/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[139] = {
#include "stg25/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg25/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg25/terrain/draw_47.inc"
};
static Sint16 draw_46_polygons[189] = {
#include "stg25/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[103] = {
#include "stg25/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg25/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg25/terrain/draw_46.inc"
};
static Sint16 draw_45_polygons[189] = {
#include "stg25/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[99] = {
#include "stg25/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg25/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg25/terrain/draw_45.inc"
};
static Sint16 draw_44_polygons[331] = {
#include "stg25/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[155] = {
#include "stg25/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg25/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg25/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[193] = {
#include "stg25/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[139] = {
#include "stg25/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg25/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg25/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[705] = {
#include "stg25/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[455] = {
#include "stg25/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg25/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg25/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[153] = {
#include "stg25/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[79] = {
#include "stg25/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg25/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg25/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[147] = {
#include "stg25/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[99] = {
#include "stg25/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg25/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg25/terrain/draw_40.inc"
};
static Sint16 draw_38_polygons[147] = {
#include "stg25/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[99] = {
#include "stg25/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg25/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg25/terrain/draw_39.inc"
};
static Sint16 draw_36_polygons[147] = {
#include "stg25/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[99] = {
#include "stg25/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg25/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg25/terrain/draw_37.inc"
};
static Sint16 draw_34_polygons[189] = {
#include "stg25/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[99] = {
#include "stg25/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg25/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg25/terrain/draw_35.inc"
};
static Sint16 draw_32_polygons[147] = {
#include "stg25/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[99] = {
#include "stg25/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg25/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg25/terrain/draw_33.inc"
};
static Sint16 draw_31_polygons[715] = {
#include "stg25/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[443] = {
#include "stg25/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg25/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg25/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[247] = {
#include "stg25/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[203] = {
#include "stg25/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg25/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg25/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[395] = {
#include "stg25/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[311] = {
#include "stg25/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg25/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg25/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[421] = {
#include "stg25/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[379] = {
#include "stg25/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg25/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg25/terrain/draw_28.inc"
};
static Sint16 draw_25_polygons[255] = {
#include "stg25/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[91] = {
#include "stg25/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg25/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg25/terrain/draw_26.inc"
};
static Sint16 draw_24_polygons[341] = {
#include "stg25/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[139] = {
#include "stg25/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg25/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg25/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[515] = {
#include "stg25/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[243] = {
#include "stg25/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg25/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg25/terrain/draw_23.inc"
};
static Sint16 draw_21_polygons[57] = {
#include "stg25/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[51] = {
#include "stg25/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg25/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg25/terrain/draw_22.inc"
};
static Sint16 draw_19_polygons[57] = {
#include "stg25/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[47] = {
#include "stg25/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg25/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg25/terrain/draw_20.inc"
};
static Sint16 draw_18_polygons[915] = {
#include "stg25/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[483] = {
#include "stg25/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg25/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg25/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[105] = {
#include "stg25/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[79] = {
#include "stg25/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg25/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg25/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[139] = {
#include "stg25/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[107] = {
#include "stg25/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg25/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg25/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[257] = {
#include "stg25/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[179] = {
#include "stg25/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg25/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg25/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[41] = {
#include "stg25/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[27] = {
#include "stg25/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg25/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg25/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[521] = {
#include "stg25/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[299] = {
#include "stg25/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg25/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg25/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[153] = {
#include "stg25/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[83] = {
#include "stg25/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg25/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg25/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[83] = {
#include "stg25/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[67] = {
#include "stg25/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg25/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg25/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[103] = {
#include "stg25/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[79] = {
#include "stg25/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg25/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg25/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[183] = {
#include "stg25/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[99] = {
#include "stg25/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg25/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg25/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[929] = {
#include "stg25/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[507] = {
#include "stg25/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg25/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg25/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[133] = {
#include "stg25/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[71] = {
#include "stg25/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg25/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg25/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[301] = {
#include "stg25/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[167] = {
#include "stg25/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg25/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg25/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[181] = {
#include "stg25/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[99] = {
#include "stg25/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg25/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg25/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[161] = {
#include "stg25/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[147] = {
#include "stg25/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg25/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg25/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[175] = {
#include "stg25/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[99] = {
#include "stg25/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg25/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg25/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[181] = {
#include "stg25/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[99] = {
#include "stg25/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg25/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg25/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[727] = {
#include "stg25/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[463] = {
#include "stg25/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg25/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg25/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[257] = {
#include "stg25/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[187] = {
#include "stg25/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg25/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg25/terrain/draw_0.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg25/terrain/collision_0.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg25/terrain/collision_3.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg25/terrain/collision_5.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg25/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg25/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg25/terrain/collision_13.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg25/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg25/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg25/terrain/collision_17.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg25/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg25/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg25/terrain/collision_21.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg25/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg25/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg25/terrain/collision_27.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg25/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg25/terrain/collision_30.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg25/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg25/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg25/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg25/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg25/terrain/collision_38.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg25/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg25/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg25/terrain/collision_44.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg25/terrain/collision_53.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg25/terrain/collision_56.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg25/terrain/collision_58.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg25/terrain/collision_60.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg25/terrain/collision_62.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg25/terrain/collision_64.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg25/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg25/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg25/terrain/collision_76.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg25/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg25/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg25/terrain/collision_80.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg25/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg25/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg25/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg25/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg25/terrain/collision_86.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg25/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg25/terrain/collision_89.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg25/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg25/terrain/collision_92.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg25/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg25/terrain/collision_99.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg25/terrain/collision_102.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg25/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg25/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg25/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg25/terrain/collision_111.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg25/terrain/collision_113.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg25/terrain/collision_115.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg25/terrain/collision_117.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg25/terrain/collision_127.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg25/terrain/collision_139.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg25/terrain/collision_141.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg25/terrain/collision_143.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg25/terrain/collision_145.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg25/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg25/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg25/terrain/collision_149.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg25/terrain/collision_150.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg25/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg25/terrain/collision_152.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg25/terrain/collision_153.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg25/terrain/collision_158.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg25/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg25/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg25/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg25/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg25/terrain/collision_166.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg25/terrain/collision_167.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg25/terrain/collision_168.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg25/terrain/collision_169.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg25/terrain/collision_170.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg25/terrain/collision_171.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg25/terrain/collision_172.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg25/terrain/collision_173.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg25/terrain/collision_174.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg25/terrain/collision_175.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg25/terrain/collision_176.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg25/terrain/collision_177.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg25/terrain/collision_178.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg25/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg25/terrain/collision_180.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg25/terrain/collision_182.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg25/terrain/collision_186.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg25/terrain/collision_187.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg25/terrain/collision_188.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg25/terrain/collision_190.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg25/terrain/collision_191.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg25/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg25/terrain/collision_193.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg25/terrain/collision_194.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg25/terrain/collision_196.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg25/terrain/collision_197.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg25/terrain/collision_199.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg25/terrain/collision_200.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg25/terrain/collision_207.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg25/terrain/collision_208.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg25/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg25/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg25/terrain/collision_211.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg25/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg25/terrain/collision_214.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg25/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg25/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg25/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg25/terrain/collision_219.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg25/terrain/collision_220.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg25/terrain/collision_223.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg25/terrain/collision_229.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg25/terrain/collision_230.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg25/terrain/collision_231.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg25/terrain/collision_232.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg25/terrain/collision_245.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg25/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg25/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg25/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg25/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg25/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg25/terrain/draw_238.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg25/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg25/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg25/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg25/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg25/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg25/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg25/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg25/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg25/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg25/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg25/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg25/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg25/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg25/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg25/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg25/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg25/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg25/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg25/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg25/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg25/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg25/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg25/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg25/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg25/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg25/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg25/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg25/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg25/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg25/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg25/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg25/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg25/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg25/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg25/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg25/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg25/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg25/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg25/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg25/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg25/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg25/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg25/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg25/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg25/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg25/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg25/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg25/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg25/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg25/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg25/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg25/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg25/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg25/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg25/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg25/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg25/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg25/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg25/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg25/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg25/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg25/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg25/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg25/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg25/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg25/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg25/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg25/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg25/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg25/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg25/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg25/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg25/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg25/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg25/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg25/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg25/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg25/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg25/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg25/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg25/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg25/terrain/draw_19.inc"
};
