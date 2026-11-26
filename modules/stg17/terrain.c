#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg17/mission_street.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char mission_street_land_filenames[1197] = {
#include "stg17/terrain/mission_street_land_filenames.inc"
};

/* STG17.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between mission_street.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[671];
struct land_table mission_street_land;
static NJS_TEXNAME textures[93];
NJS_TEXLIST mission_street_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[94];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[49];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[10];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[8];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[10];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[8];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[10];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[8];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[100];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[53];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[28];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[26];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[132];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[78];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[20];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[14];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[38];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[23];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[10];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[8];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[10];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[10];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[8];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[10];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[8];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[10];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[8];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[10];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[8];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[10];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[8];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[10];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[8];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[10];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[8];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[10];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[8];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[10];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[38];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[36];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[14];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[12];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[20];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[12];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[14];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[9];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[14];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[12];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[14];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[12];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[14];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[12];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[18];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[16];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[18];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[16];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[38];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[36];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[14];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[12];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[132];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[78];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[17];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[12];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[10];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[8];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[10];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[8];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[80];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[54];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[22];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[20];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[14];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[12];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[17];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[8];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[30];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[16];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[30];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[16];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[28];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[16];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[5];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[4];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[14];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[8];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[19];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[16];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[15];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[12];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[19];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[8];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[73];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[44];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[26];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[18];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[26];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[18];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[26];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[18];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[26];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[18];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[26];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[18];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[26];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[18];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[33];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[26];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[31];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[16];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[31];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[16];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_93_materials[1];
static Sint16 collision_93_faces[31];
static NJS_MESHSET collision_93_meshsets[1];
static NJS_POINT3 collision_93_points[16];
static NJS_MODEL collision_93_model;
static NJS_OBJECT collision_93;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[31];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[16];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[31];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[16];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_96_materials[1];
static Sint16 collision_96_faces[31];
static NJS_MESHSET collision_96_meshsets[1];
static NJS_POINT3 collision_96_points[16];
static NJS_MODEL collision_96_model;
static NJS_OBJECT collision_96;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[7];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[6];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_99_materials[1];
static Sint16 collision_99_faces[28];
static NJS_MESHSET collision_99_meshsets[1];
static NJS_POINT3 collision_99_points[16];
static NJS_MODEL collision_99_model;
static NJS_OBJECT collision_99;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[56];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[25];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[22];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[12];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[22];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[12];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[22];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[12];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[22];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[12];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[22];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[12];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[22];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[12];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[22];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[12];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[22];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[12];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[22];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[12];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[57];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[54];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[35];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[24];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[136];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[60];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[33];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[26];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_132_materials[1];
static Sint16 collision_132_faces[36];
static NJS_MESHSET collision_132_meshsets[1];
static NJS_POINT3 collision_132_points[19];
static NJS_MODEL collision_132_model;
static NJS_OBJECT collision_132;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[36];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[19];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[10];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[6];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[21];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[12];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[21];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[18];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[35];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[24];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[28];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[21];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[28];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[21];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[33];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[26];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_141_materials[1];
static Sint16 collision_141_faces[25];
static NJS_MESHSET collision_141_meshsets[1];
static NJS_POINT3 collision_141_points[15];
static NJS_MODEL collision_141_model;
static NJS_OBJECT collision_141;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[27];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[24];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[27];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[24];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[11];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[8];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_145_materials[1];
static Sint16 collision_145_faces[14];
static NJS_MESHSET collision_145_meshsets[1];
static NJS_POINT3 collision_145_points[9];
static NJS_MODEL collision_145_model;
static NJS_OBJECT collision_145;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[5];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[4];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_147_materials[1];
static Sint16 collision_147_faces[34];
static NJS_MESHSET collision_147_meshsets[1];
static NJS_POINT3 collision_147_points[16];
static NJS_MODEL collision_147_model;
static NJS_OBJECT collision_147;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[57];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[54];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[16];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[8];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[19];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[8];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_165_materials[1];
static Sint16 collision_165_faces[33];
static NJS_MESHSET collision_165_meshsets[1];
static NJS_POINT3 collision_165_points[26];
static NJS_MODEL collision_165_model;
static NJS_OBJECT collision_165;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[28];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[21];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[15];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[12];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[15];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[12];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[58];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[24];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[10];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[8];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[10];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[8];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[10];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[8];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[10];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[8];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[10];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[8];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[61];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[30];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_180_materials[1];
static Sint16 collision_180_faces[52];
static NJS_MESHSET collision_180_meshsets[1];
static NJS_POINT3 collision_180_points[30];
static NJS_MODEL collision_180_model;
static NJS_OBJECT collision_180;
static NJS_MATERIAL collision_184_materials[1];
static Sint16 collision_184_faces[49];
static NJS_MESHSET collision_184_meshsets[1];
static NJS_POINT3 collision_184_points[24];
static NJS_MODEL collision_184_model;
static NJS_OBJECT collision_184;
static NJS_MATERIAL collision_185_materials[1];
static Sint16 collision_185_faces[21];
static NJS_MESHSET collision_185_meshsets[1];
static NJS_POINT3 collision_185_points[20];
static NJS_MODEL collision_185_model;
static NJS_OBJECT collision_185;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[28];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[15];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_192_materials[1];
static Sint16 collision_192_faces[22];
static NJS_MESHSET collision_192_meshsets[1];
static NJS_POINT3 collision_192_points[12];
static NJS_MODEL collision_192_model;
static NJS_OBJECT collision_192;
static NJS_MATERIAL collision_193_materials[1];
static Sint16 collision_193_faces[51];
static NJS_MESHSET collision_193_meshsets[1];
static NJS_POINT3 collision_193_points[24];
static NJS_MODEL collision_193_model;
static NJS_OBJECT collision_193;
static Sint16 draw_9_polygons[543];
static Sint32 draw_9_vertices[399];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_471;
static Sint16 draw_19_polygons[193];
static Sint32 draw_19_vertices[155];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_470;
static Sint16 draw_469_polygons[73];
static Sint32 draw_469_vertices[51];
static NJS_CNK_MODEL draw_469_model;
static NJS_CNK_OBJECT draw_469;
static Sint16 draw_403_polygons[55];
static Sint32 draw_403_vertices[67];
static NJS_CNK_MODEL draw_403_model;
static NJS_CNK_OBJECT draw_468;
static Sint16 draw_17_polygons[19];
static Sint32 draw_17_vertices[19];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_467;
static Sint16 draw_16_polygons[191];
static Sint32 draw_16_vertices[127];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_460;
static Sint16 draw_14_polygons[89];
static Sint32 draw_14_vertices[59];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_452;
static Sint16 draw_13_polygons[19];
static Sint32 draw_13_vertices[19];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_450;
static Sint16 draw_41_polygons[19];
static Sint32 draw_41_vertices[19];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_449;
static Sint16 draw_40_polygons[43];
static Sint32 draw_40_vertices[31];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_448;
static Sint16 draw_39_polygons[81];
static Sint32 draw_39_vertices[75];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_446;
static Sint16 draw_11_polygons[217];
static Sint32 draw_11_vertices[127];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_440;
static Sint16 draw_10_polygons[265];
static Sint32 draw_10_vertices[195];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_433;
static Sint16 draw_7_polygons[151];
static Sint32 draw_7_vertices[103];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_428;
static Sint16 draw_425_polygons[43];
static Sint32 draw_425_vertices[35];
static NJS_CNK_MODEL draw_425_model;
static NJS_CNK_OBJECT draw_425;
static Sint16 draw_5_polygons[167];
static Sint32 draw_5_vertices[123];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_424;
static Sint16 draw_3_polygons[1413];
static Sint32 draw_3_vertices[1235];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_421;
static Sint16 draw_2_polygons[33];
static Sint32 draw_2_vertices[35];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_414;
static Sint16 draw_1_polygons[121];
static Sint32 draw_1_vertices[67];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_410;
static Sint16 draw_0_polygons[107];
static Sint32 draw_0_vertices[67];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_407;
static Sint16 draw_37_polygons[33];
static Sint32 draw_37_vertices[35];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_405;
static Sint16 draw_46_polygons[207];
static Sint32 draw_46_vertices[111];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_402;
static Sint16 draw_26_polygons[93];
static Sint32 draw_26_vertices[115];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_398;
static Sint16 draw_96_polygons[503];
static Sint32 draw_96_vertices[371];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_394;
static Sint16 draw_15_polygons[89];
static Sint32 draw_15_vertices[59];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_389;
static Sint16 draw_12_polygons[93];
static Sint32 draw_12_vertices[83];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_388;
static Sint16 draw_21_polygons[241];
static Sint32 draw_21_vertices[183];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_387;
static Sint16 draw_8_polygons[515];
static Sint32 draw_8_vertices[447];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_386;
static Sint16 draw_25_polygons[1245];
static Sint32 draw_25_vertices[1247];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_385;
static Sint16 draw_92_polygons[293];
static Sint32 draw_92_vertices[363];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_384;
static Sint16 draw_98_polygons[157];
static Sint32 draw_98_vertices[87];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_383;
static Sint16 draw_382_polygons[403];
static Sint32 draw_382_vertices[259];
static NJS_CNK_MODEL draw_382_model;
static NJS_CNK_OBJECT draw_382;
static Sint16 draw_86_polygons[257];
static Sint32 draw_86_vertices[147];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_381;
static Sint16 draw_84_polygons[87];
static Sint32 draw_84_vertices[67];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_380;
static Sint16 draw_83_polygons[659];
static Sint32 draw_83_vertices[507];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_379;
static Sint16 draw_82_polygons[425];
static Sint32 draw_82_vertices[295];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_378;
static Sint16 draw_93_polygons[153];
static Sint32 draw_93_vertices[139];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_377;
static Sint16 draw_91_polygons[679];
static Sint32 draw_91_vertices[519];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_376;
static Sint16 draw_127_polygons[121];
static Sint32 draw_127_vertices[75];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_375;
static Sint16 draw_113_polygons[273];
static Sint32 draw_113_vertices[219];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_370;
static Sint16 draw_328_polygons[89];
static Sint32 draw_328_vertices[99];
static NJS_CNK_MODEL draw_328_model;
static NJS_CNK_OBJECT draw_329;
static Sint16 draw_114_polygons[679];
static Sint32 draw_114_vertices[511];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_325;
static Sint16 draw_311_polygons[433];
static Sint32 draw_311_vertices[355];
static NJS_CNK_MODEL draw_311_model;
static NJS_CNK_OBJECT draw_311;
static Sint16 draw_309_polygons[705];
static Sint32 draw_309_vertices[531];
static NJS_CNK_MODEL draw_309_model;
static NJS_CNK_OBJECT draw_310;
static Sint16 draw_307_polygons[75];
static Sint32 draw_307_vertices[51];
static NJS_CNK_MODEL draw_307_model;
static NJS_CNK_OBJECT draw_308;
static Sint16 draw_305_polygons[705];
static Sint32 draw_305_vertices[531];
static NJS_CNK_MODEL draw_305_model;
static NJS_CNK_OBJECT draw_306;
static Sint16 draw_304_polygons[153];
static Sint32 draw_304_vertices[107];
static NJS_CNK_MODEL draw_304_model;
static NJS_CNK_OBJECT draw_304;
static Sint16 draw_303_polygons[61];
static Sint32 draw_303_vertices[75];
static NJS_CNK_MODEL draw_303_model;
static NJS_CNK_OBJECT draw_303;
static Sint16 draw_110_polygons[171];
static Sint32 draw_110_vertices[107];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_302;
static Sint16 draw_293_polygons[513];
static Sint32 draw_293_vertices[351];
static NJS_CNK_MODEL draw_293_model;
static NJS_CNK_OBJECT draw_293;
static Sint16 draw_108_polygons[249];
static Sint32 draw_108_vertices[155];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_292;
static Sint16 draw_285_polygons[505];
static Sint32 draw_285_vertices[327];
static NJS_CNK_MODEL draw_285_model;
static NJS_CNK_OBJECT draw_285;
static Sint16 draw_81_polygons[319];
static Sint32 draw_81_vertices[163];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_284;
static Sint16 draw_100_polygons[121];
static Sint32 draw_100_vertices[75];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_273;
static Sint16 draw_272_polygons[121];
static Sint32 draw_272_vertices[75];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_272;
static Sint16 draw_99_polygons[33];
static Sint32 draw_99_vertices[35];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_271;
static Sint16 draw_263_polygons[631];
static Sint32 draw_263_vertices[571];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_264;
static Sint16 draw_251_polygons[97];
static Sint32 draw_251_vertices[63];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_262;
static Sint16 draw_248_polygons[425];
static Sint32 draw_248_vertices[515];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_175_polygons[187];
static Sint32 draw_175_vertices[203];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_241_polygons[753];
static Sint32 draw_241_vertices[495];
static NJS_CNK_MODEL draw_241_model;
static NJS_CNK_OBJECT draw_243;
static Sint16 draw_240_polygons[215];
static Sint32 draw_240_vertices[103];
static NJS_CNK_MODEL draw_240_model;
static NJS_CNK_OBJECT draw_240;
static Sint16 draw_239_polygons[151];
static Sint32 draw_239_vertices[99];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_239;
static Sint16 draw_236_polygons[429];
static Sint32 draw_236_vertices[515];
static NJS_CNK_MODEL draw_236_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_166_polygons[233];
static Sint32 draw_166_vertices[251];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_229_polygons[753];
static Sint32 draw_229_vertices[495];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_231;
static Sint16 draw_165_polygons[19];
static Sint32 draw_165_vertices[19];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_228;
static Sint16 draw_222_polygons[333];
static Sint32 draw_222_vertices[347];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_227;
static Sint16 draw_225_polygons[1885];
static Sint32 draw_225_vertices[1539];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_218_polygons[95];
static Sint32 draw_218_vertices[91];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_221;
static Sint16 draw_212_polygons[333];
static Sint32 draw_212_vertices[347];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_193_polygons[821];
static Sint32 draw_193_vertices[587];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_190_polygons[49];
static Sint32 draw_190_vertices[51];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_185_polygons[57];
static Sint32 draw_185_vertices[43];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_184_polygons[663];
static Sint32 draw_184_vertices[555];
static NJS_CNK_MODEL draw_184_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_151_polygons[263];
static Sint32 draw_151_vertices[203];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_144_polygons[245];
static Sint32 draw_144_vertices[159];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_139_polygons[121];
static Sint32 draw_139_vertices[91];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_134_polygons[121];
static Sint32 draw_134_vertices[75];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_129_polygons[83];
static Sint32 draw_129_vertices[67];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_97_polygons[451];
static Sint32 draw_97_vertices[351];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_122_polygons[505];
static Sint32 draw_122_vertices[327];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_95_polygons[89];
static Sint32 draw_95_vertices[67];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_112_polygons[513];
static Sint32 draw_112_vertices[351];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_94_polygons[19];
static Sint32 draw_94_vertices[19];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_90_polygons[213];
static Sint32 draw_90_vertices[151];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_89_polygons[403];
static Sint32 draw_89_vertices[259];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_88_polygons[301];
static Sint32 draw_88_vertices[255];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
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
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_107;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_156;
static NJS_OBJECT collision_157;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_159;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_169;
static NJS_OBJECT collision_171;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_181;
static NJS_OBJECT collision_182;
static NJS_OBJECT collision_183;
static NJS_OBJECT collision_186;
static NJS_OBJECT collision_187;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_189;
static NJS_OBJECT collision_190;
static NJS_CNK_OBJECT draw_476;
static NJS_CNK_OBJECT draw_475;
static NJS_CNK_OBJECT draw_474;
static NJS_CNK_OBJECT draw_473;
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_466;
static NJS_CNK_OBJECT draw_465;
static NJS_CNK_OBJECT draw_464;
static NJS_CNK_OBJECT draw_463;
static NJS_CNK_OBJECT draw_462;
static NJS_CNK_OBJECT draw_461;
static NJS_CNK_OBJECT draw_459;
static NJS_CNK_OBJECT draw_458;
static NJS_CNK_OBJECT draw_457;
static NJS_CNK_OBJECT draw_456;
static NJS_CNK_OBJECT draw_455;
static NJS_CNK_OBJECT draw_454;
static NJS_CNK_OBJECT draw_453;
static NJS_CNK_OBJECT draw_451;
static NJS_CNK_OBJECT draw_447;
static NJS_CNK_OBJECT draw_445;
static NJS_CNK_OBJECT draw_444;
static NJS_CNK_OBJECT draw_443;
static NJS_CNK_OBJECT draw_442;
static NJS_CNK_OBJECT draw_441;
static NJS_CNK_OBJECT draw_439;
static NJS_CNK_OBJECT draw_438;
static NJS_CNK_OBJECT draw_437;
static NJS_CNK_OBJECT draw_436;
static NJS_CNK_OBJECT draw_435;
static NJS_CNK_OBJECT draw_434;
static NJS_CNK_OBJECT draw_432;
static NJS_CNK_OBJECT draw_431;
static NJS_CNK_OBJECT draw_430;
static NJS_CNK_OBJECT draw_429;
static NJS_CNK_OBJECT draw_427;
static NJS_CNK_OBJECT draw_426;
static NJS_CNK_OBJECT draw_423;
static NJS_CNK_OBJECT draw_422;
static NJS_CNK_OBJECT draw_420;
static NJS_CNK_OBJECT draw_419;
static NJS_CNK_OBJECT draw_418;
static NJS_CNK_OBJECT draw_417;
static NJS_CNK_OBJECT draw_416;
static NJS_CNK_OBJECT draw_415;
static NJS_CNK_OBJECT draw_413;
static NJS_CNK_OBJECT draw_412;
static NJS_CNK_OBJECT draw_411;
static NJS_CNK_OBJECT draw_409;
static NJS_CNK_OBJECT draw_408;
static NJS_CNK_OBJECT draw_406;
static NJS_CNK_OBJECT draw_404;
static NJS_CNK_OBJECT draw_403;
static NJS_CNK_OBJECT draw_401;
static NJS_CNK_OBJECT draw_400;
static NJS_CNK_OBJECT draw_399;
static NJS_CNK_OBJECT draw_397;
static NJS_CNK_OBJECT draw_396;
static NJS_CNK_OBJECT draw_395;
static NJS_CNK_OBJECT draw_393;
static NJS_CNK_OBJECT draw_392;
static NJS_CNK_OBJECT draw_391;
static NJS_CNK_OBJECT draw_390;
static NJS_CNK_OBJECT draw_374;
static NJS_CNK_OBJECT draw_373;
static NJS_CNK_OBJECT draw_372;
static NJS_CNK_OBJECT draw_371;
static NJS_CNK_OBJECT draw_369;
static NJS_CNK_OBJECT draw_368;
static NJS_CNK_OBJECT draw_367;
static NJS_CNK_OBJECT draw_366;
static NJS_CNK_OBJECT draw_365;
static NJS_CNK_OBJECT draw_364;
static NJS_CNK_OBJECT draw_363;
static NJS_CNK_OBJECT draw_362;
static NJS_CNK_OBJECT draw_361;
static NJS_CNK_OBJECT draw_360;
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
static NJS_CNK_OBJECT draw_328;
static NJS_CNK_OBJECT draw_327;
static NJS_CNK_OBJECT draw_326;
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
static NJS_CNK_OBJECT draw_309;
static NJS_CNK_OBJECT draw_307;
static NJS_CNK_OBJECT draw_305;
static NJS_CNK_OBJECT draw_301;
static NJS_CNK_OBJECT draw_300;
static NJS_CNK_OBJECT draw_299;
static NJS_CNK_OBJECT draw_298;
static NJS_CNK_OBJECT draw_297;
static NJS_CNK_OBJECT draw_296;
static NJS_CNK_OBJECT draw_295;
static NJS_CNK_OBJECT draw_294;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_290;
static NJS_CNK_OBJECT draw_289;
static NJS_CNK_OBJECT draw_288;
static NJS_CNK_OBJECT draw_287;
static NJS_CNK_OBJECT draw_286;
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
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_269;
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_267;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_263;
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
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_248;
static NJS_CNK_OBJECT draw_246;
static NJS_CNK_OBJECT draw_245;
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_233;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_229;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_218;
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
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
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
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
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
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
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
static NJS_CNK_OBJECT draw_66;
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

static struct land_entry entries[671] = {
#include "stg17/terrain/entries.inc"
};
struct land_table mission_street_land = {
#include "stg17/terrain/mission_street_land.inc"
};
static NJS_TEXNAME textures[93] = {
#include "stg17/terrain/textures.inc"
};
NJS_TEXLIST mission_street_land_texlist = {
#include "stg17/terrain/mission_street_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg17/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[94] = {
#include "stg17/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg17/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[49] = {
#include "stg17/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg17/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg17/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg17/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[10] = {
#include "stg17/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg17/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[8] = {
#include "stg17/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg17/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg17/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg17/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[10] = {
#include "stg17/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg17/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[8] = {
#include "stg17/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg17/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg17/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg17/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[10] = {
#include "stg17/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg17/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[8] = {
#include "stg17/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg17/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg17/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg17/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[100] = {
#include "stg17/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg17/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[53] = {
#include "stg17/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg17/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg17/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg17/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[28] = {
#include "stg17/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg17/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[26] = {
#include "stg17/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg17/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg17/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg17/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[132] = {
#include "stg17/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg17/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[78] = {
#include "stg17/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg17/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg17/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg17/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[20] = {
#include "stg17/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg17/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[14] = {
#include "stg17/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg17/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg17/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg17/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[38] = {
#include "stg17/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg17/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[23] = {
#include "stg17/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg17/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg17/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg17/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[10] = {
#include "stg17/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg17/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[8] = {
#include "stg17/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg17/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg17/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg17/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[10] = {
#include "stg17/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg17/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg17/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg17/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg17/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg17/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[10] = {
#include "stg17/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg17/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[8] = {
#include "stg17/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg17/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg17/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg17/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[10] = {
#include "stg17/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg17/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[8] = {
#include "stg17/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg17/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg17/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg17/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[10] = {
#include "stg17/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg17/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[8] = {
#include "stg17/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg17/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg17/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg17/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[10] = {
#include "stg17/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg17/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[8] = {
#include "stg17/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg17/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg17/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg17/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[10] = {
#include "stg17/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg17/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[8] = {
#include "stg17/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg17/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg17/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg17/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[10] = {
#include "stg17/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg17/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[8] = {
#include "stg17/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg17/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg17/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg17/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[10] = {
#include "stg17/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg17/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[8] = {
#include "stg17/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg17/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg17/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg17/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[10] = {
#include "stg17/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg17/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[8] = {
#include "stg17/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg17/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg17/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg17/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[10] = {
#include "stg17/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg17/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg17/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg17/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg17/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg17/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[38] = {
#include "stg17/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg17/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[36] = {
#include "stg17/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg17/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg17/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg17/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[14] = {
#include "stg17/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg17/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[12] = {
#include "stg17/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg17/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg17/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg17/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[20] = {
#include "stg17/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg17/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[12] = {
#include "stg17/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg17/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg17/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg17/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[14] = {
#include "stg17/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg17/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[9] = {
#include "stg17/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg17/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg17/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg17/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[14] = {
#include "stg17/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg17/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[12] = {
#include "stg17/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg17/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg17/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg17/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[14] = {
#include "stg17/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg17/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[12] = {
#include "stg17/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg17/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg17/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg17/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[14] = {
#include "stg17/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg17/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[12] = {
#include "stg17/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg17/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg17/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg17/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[18] = {
#include "stg17/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg17/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[16] = {
#include "stg17/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg17/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg17/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg17/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[18] = {
#include "stg17/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg17/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[16] = {
#include "stg17/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg17/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg17/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg17/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[38] = {
#include "stg17/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg17/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[36] = {
#include "stg17/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg17/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg17/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg17/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[14] = {
#include "stg17/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg17/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[12] = {
#include "stg17/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg17/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg17/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg17/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[132] = {
#include "stg17/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg17/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[78] = {
#include "stg17/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg17/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg17/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg17/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[17] = {
#include "stg17/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg17/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[12] = {
#include "stg17/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg17/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg17/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg17/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[10] = {
#include "stg17/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg17/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[8] = {
#include "stg17/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg17/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg17/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg17/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[10] = {
#include "stg17/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg17/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[8] = {
#include "stg17/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg17/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg17/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg17/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[80] = {
#include "stg17/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg17/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[54] = {
#include "stg17/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg17/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg17/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg17/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[22] = {
#include "stg17/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg17/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[20] = {
#include "stg17/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg17/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg17/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg17/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[14] = {
#include "stg17/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg17/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[12] = {
#include "stg17/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg17/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg17/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg17/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[17] = {
#include "stg17/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg17/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[8] = {
#include "stg17/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg17/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg17/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg17/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[30] = {
#include "stg17/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg17/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[16] = {
#include "stg17/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg17/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg17/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg17/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[30] = {
#include "stg17/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg17/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[16] = {
#include "stg17/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg17/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg17/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg17/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[28] = {
#include "stg17/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg17/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[16] = {
#include "stg17/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg17/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg17/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg17/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[5] = {
#include "stg17/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg17/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[4] = {
#include "stg17/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg17/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg17/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg17/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[14] = {
#include "stg17/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg17/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[8] = {
#include "stg17/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg17/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg17/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg17/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[19] = {
#include "stg17/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg17/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[16] = {
#include "stg17/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg17/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg17/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg17/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[15] = {
#include "stg17/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg17/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[12] = {
#include "stg17/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg17/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg17/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg17/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[19] = {
#include "stg17/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg17/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[8] = {
#include "stg17/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg17/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg17/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg17/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[73] = {
#include "stg17/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg17/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[44] = {
#include "stg17/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg17/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg17/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg17/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[26] = {
#include "stg17/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg17/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[18] = {
#include "stg17/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg17/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg17/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg17/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[26] = {
#include "stg17/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg17/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[18] = {
#include "stg17/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg17/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg17/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg17/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[26] = {
#include "stg17/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg17/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[18] = {
#include "stg17/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg17/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg17/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg17/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[26] = {
#include "stg17/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg17/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[18] = {
#include "stg17/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg17/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg17/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg17/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[26] = {
#include "stg17/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg17/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[18] = {
#include "stg17/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg17/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg17/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg17/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[26] = {
#include "stg17/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg17/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[18] = {
#include "stg17/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg17/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg17/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg17/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[33] = {
#include "stg17/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg17/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[26] = {
#include "stg17/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg17/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg17/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg17/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[31] = {
#include "stg17/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg17/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[16] = {
#include "stg17/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg17/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg17/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg17/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[31] = {
#include "stg17/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg17/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[16] = {
#include "stg17/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg17/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg17/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_93_materials[1] = {
#include "stg17/terrain/collision_93_materials.inc"
};
static Sint16 collision_93_faces[31] = {
#include "stg17/terrain/collision_93_faces.inc"
};
static NJS_MESHSET collision_93_meshsets[1] = {
#include "stg17/terrain/collision_93_meshsets.inc"
};
static NJS_POINT3 collision_93_points[16] = {
#include "stg17/terrain/collision_93_points.inc"
};
static NJS_MODEL collision_93_model = {
#include "stg17/terrain/collision_93_model.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg17/terrain/collision_93.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg17/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[31] = {
#include "stg17/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg17/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[16] = {
#include "stg17/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg17/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg17/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg17/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[31] = {
#include "stg17/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg17/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[16] = {
#include "stg17/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg17/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg17/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_96_materials[1] = {
#include "stg17/terrain/collision_96_materials.inc"
};
static Sint16 collision_96_faces[31] = {
#include "stg17/terrain/collision_96_faces.inc"
};
static NJS_MESHSET collision_96_meshsets[1] = {
#include "stg17/terrain/collision_96_meshsets.inc"
};
static NJS_POINT3 collision_96_points[16] = {
#include "stg17/terrain/collision_96_points.inc"
};
static NJS_MODEL collision_96_model = {
#include "stg17/terrain/collision_96_model.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg17/terrain/collision_96.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg17/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[7] = {
#include "stg17/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg17/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[6] = {
#include "stg17/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg17/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg17/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_99_materials[1] = {
#include "stg17/terrain/collision_99_materials.inc"
};
static Sint16 collision_99_faces[28] = {
#include "stg17/terrain/collision_99_faces.inc"
};
static NJS_MESHSET collision_99_meshsets[1] = {
#include "stg17/terrain/collision_99_meshsets.inc"
};
static NJS_POINT3 collision_99_points[16] = {
#include "stg17/terrain/collision_99_points.inc"
};
static NJS_MODEL collision_99_model = {
#include "stg17/terrain/collision_99_model.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg17/terrain/collision_99.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg17/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[56] = {
#include "stg17/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg17/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[25] = {
#include "stg17/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg17/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg17/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg17/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[22] = {
#include "stg17/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg17/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[12] = {
#include "stg17/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg17/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg17/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg17/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[22] = {
#include "stg17/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg17/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[12] = {
#include "stg17/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg17/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg17/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg17/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[22] = {
#include "stg17/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg17/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[12] = {
#include "stg17/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg17/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg17/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg17/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[22] = {
#include "stg17/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg17/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[12] = {
#include "stg17/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg17/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg17/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg17/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[22] = {
#include "stg17/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg17/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[12] = {
#include "stg17/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg17/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg17/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg17/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[22] = {
#include "stg17/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg17/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[12] = {
#include "stg17/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg17/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg17/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg17/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[22] = {
#include "stg17/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg17/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[12] = {
#include "stg17/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg17/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg17/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg17/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[22] = {
#include "stg17/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg17/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[12] = {
#include "stg17/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg17/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg17/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg17/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[22] = {
#include "stg17/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg17/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[12] = {
#include "stg17/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg17/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg17/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg17/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[57] = {
#include "stg17/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg17/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[54] = {
#include "stg17/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg17/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg17/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg17/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[35] = {
#include "stg17/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg17/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[24] = {
#include "stg17/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg17/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg17/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg17/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[136] = {
#include "stg17/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg17/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[60] = {
#include "stg17/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg17/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg17/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg17/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[33] = {
#include "stg17/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg17/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[26] = {
#include "stg17/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg17/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg17/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_132_materials[1] = {
#include "stg17/terrain/collision_132_materials.inc"
};
static Sint16 collision_132_faces[36] = {
#include "stg17/terrain/collision_132_faces.inc"
};
static NJS_MESHSET collision_132_meshsets[1] = {
#include "stg17/terrain/collision_132_meshsets.inc"
};
static NJS_POINT3 collision_132_points[19] = {
#include "stg17/terrain/collision_132_points.inc"
};
static NJS_MODEL collision_132_model = {
#include "stg17/terrain/collision_132_model.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg17/terrain/collision_132.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg17/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[36] = {
#include "stg17/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg17/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[19] = {
#include "stg17/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg17/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg17/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg17/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[10] = {
#include "stg17/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg17/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[6] = {
#include "stg17/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg17/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg17/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg17/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[21] = {
#include "stg17/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg17/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[12] = {
#include "stg17/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg17/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg17/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg17/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[21] = {
#include "stg17/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg17/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[18] = {
#include "stg17/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg17/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg17/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg17/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[35] = {
#include "stg17/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg17/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[24] = {
#include "stg17/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg17/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg17/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg17/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[28] = {
#include "stg17/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg17/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[21] = {
#include "stg17/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg17/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg17/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg17/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[28] = {
#include "stg17/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg17/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[21] = {
#include "stg17/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg17/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg17/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg17/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[33] = {
#include "stg17/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg17/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[26] = {
#include "stg17/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg17/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg17/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_141_materials[1] = {
#include "stg17/terrain/collision_141_materials.inc"
};
static Sint16 collision_141_faces[25] = {
#include "stg17/terrain/collision_141_faces.inc"
};
static NJS_MESHSET collision_141_meshsets[1] = {
#include "stg17/terrain/collision_141_meshsets.inc"
};
static NJS_POINT3 collision_141_points[15] = {
#include "stg17/terrain/collision_141_points.inc"
};
static NJS_MODEL collision_141_model = {
#include "stg17/terrain/collision_141_model.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg17/terrain/collision_141.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg17/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[27] = {
#include "stg17/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg17/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[24] = {
#include "stg17/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg17/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg17/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg17/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[27] = {
#include "stg17/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg17/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[24] = {
#include "stg17/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg17/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg17/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg17/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[11] = {
#include "stg17/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg17/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[8] = {
#include "stg17/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg17/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg17/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_145_materials[1] = {
#include "stg17/terrain/collision_145_materials.inc"
};
static Sint16 collision_145_faces[14] = {
#include "stg17/terrain/collision_145_faces.inc"
};
static NJS_MESHSET collision_145_meshsets[1] = {
#include "stg17/terrain/collision_145_meshsets.inc"
};
static NJS_POINT3 collision_145_points[9] = {
#include "stg17/terrain/collision_145_points.inc"
};
static NJS_MODEL collision_145_model = {
#include "stg17/terrain/collision_145_model.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg17/terrain/collision_145.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg17/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[5] = {
#include "stg17/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg17/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[4] = {
#include "stg17/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg17/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg17/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_147_materials[1] = {
#include "stg17/terrain/collision_147_materials.inc"
};
static Sint16 collision_147_faces[34] = {
#include "stg17/terrain/collision_147_faces.inc"
};
static NJS_MESHSET collision_147_meshsets[1] = {
#include "stg17/terrain/collision_147_meshsets.inc"
};
static NJS_POINT3 collision_147_points[16] = {
#include "stg17/terrain/collision_147_points.inc"
};
static NJS_MODEL collision_147_model = {
#include "stg17/terrain/collision_147_model.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg17/terrain/collision_147.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg17/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[57] = {
#include "stg17/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg17/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[54] = {
#include "stg17/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg17/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg17/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg17/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[16] = {
#include "stg17/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg17/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[8] = {
#include "stg17/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg17/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg17/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg17/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[19] = {
#include "stg17/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg17/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[8] = {
#include "stg17/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg17/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg17/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_165_materials[1] = {
#include "stg17/terrain/collision_165_materials.inc"
};
static Sint16 collision_165_faces[33] = {
#include "stg17/terrain/collision_165_faces.inc"
};
static NJS_MESHSET collision_165_meshsets[1] = {
#include "stg17/terrain/collision_165_meshsets.inc"
};
static NJS_POINT3 collision_165_points[26] = {
#include "stg17/terrain/collision_165_points.inc"
};
static NJS_MODEL collision_165_model = {
#include "stg17/terrain/collision_165_model.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg17/terrain/collision_165.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg17/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[28] = {
#include "stg17/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg17/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[21] = {
#include "stg17/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg17/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg17/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg17/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[15] = {
#include "stg17/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg17/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[12] = {
#include "stg17/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg17/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg17/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg17/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[15] = {
#include "stg17/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg17/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[12] = {
#include "stg17/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg17/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg17/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg17/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[58] = {
#include "stg17/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg17/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[24] = {
#include "stg17/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg17/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg17/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg17/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[10] = {
#include "stg17/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg17/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[8] = {
#include "stg17/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg17/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg17/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg17/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[10] = {
#include "stg17/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg17/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[8] = {
#include "stg17/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg17/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg17/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg17/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[10] = {
#include "stg17/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg17/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[8] = {
#include "stg17/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg17/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg17/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg17/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[10] = {
#include "stg17/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg17/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[8] = {
#include "stg17/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg17/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg17/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg17/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[10] = {
#include "stg17/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg17/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[8] = {
#include "stg17/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg17/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg17/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg17/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[61] = {
#include "stg17/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg17/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[30] = {
#include "stg17/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg17/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg17/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_180_materials[1] = {
#include "stg17/terrain/collision_180_materials.inc"
};
static Sint16 collision_180_faces[52] = {
#include "stg17/terrain/collision_180_faces.inc"
};
static NJS_MESHSET collision_180_meshsets[1] = {
#include "stg17/terrain/collision_180_meshsets.inc"
};
static NJS_POINT3 collision_180_points[30] = {
#include "stg17/terrain/collision_180_points.inc"
};
static NJS_MODEL collision_180_model = {
#include "stg17/terrain/collision_180_model.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg17/terrain/collision_180.inc"
};
static NJS_MATERIAL collision_184_materials[1] = {
#include "stg17/terrain/collision_184_materials.inc"
};
static Sint16 collision_184_faces[49] = {
#include "stg17/terrain/collision_184_faces.inc"
};
static NJS_MESHSET collision_184_meshsets[1] = {
#include "stg17/terrain/collision_184_meshsets.inc"
};
static NJS_POINT3 collision_184_points[24] = {
#include "stg17/terrain/collision_184_points.inc"
};
static NJS_MODEL collision_184_model = {
#include "stg17/terrain/collision_184_model.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg17/terrain/collision_184.inc"
};
static NJS_MATERIAL collision_185_materials[1] = {
#include "stg17/terrain/collision_185_materials.inc"
};
static Sint16 collision_185_faces[21] = {
#include "stg17/terrain/collision_185_faces.inc"
};
static NJS_MESHSET collision_185_meshsets[1] = {
#include "stg17/terrain/collision_185_meshsets.inc"
};
static NJS_POINT3 collision_185_points[20] = {
#include "stg17/terrain/collision_185_points.inc"
};
static NJS_MODEL collision_185_model = {
#include "stg17/terrain/collision_185_model.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg17/terrain/collision_185.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg17/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[28] = {
#include "stg17/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg17/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[15] = {
#include "stg17/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg17/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg17/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_192_materials[1] = {
#include "stg17/terrain/collision_192_materials.inc"
};
static Sint16 collision_192_faces[22] = {
#include "stg17/terrain/collision_192_faces.inc"
};
static NJS_MESHSET collision_192_meshsets[1] = {
#include "stg17/terrain/collision_192_meshsets.inc"
};
static NJS_POINT3 collision_192_points[12] = {
#include "stg17/terrain/collision_192_points.inc"
};
static NJS_MODEL collision_192_model = {
#include "stg17/terrain/collision_192_model.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg17/terrain/collision_192.inc"
};
static NJS_MATERIAL collision_193_materials[1] = {
#include "stg17/terrain/collision_193_materials.inc"
};
static Sint16 collision_193_faces[51] = {
#include "stg17/terrain/collision_193_faces.inc"
};
static NJS_MESHSET collision_193_meshsets[1] = {
#include "stg17/terrain/collision_193_meshsets.inc"
};
static NJS_POINT3 collision_193_points[24] = {
#include "stg17/terrain/collision_193_points.inc"
};
static NJS_MODEL collision_193_model = {
#include "stg17/terrain/collision_193_model.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg17/terrain/collision_193.inc"
};
static Sint16 draw_9_polygons[543] = {
#include "stg17/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[399] = {
#include "stg17/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg17/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg17/terrain/draw_471.inc"
};
static Sint16 draw_19_polygons[193] = {
#include "stg17/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[155] = {
#include "stg17/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg17/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg17/terrain/draw_470.inc"
};
static Sint16 draw_469_polygons[73] = {
#include "stg17/terrain/draw_469_polygons.inc"
};
static Sint32 draw_469_vertices[51] = {
#include "stg17/terrain/draw_469_vertices.inc"
};
static NJS_CNK_MODEL draw_469_model = {
#include "stg17/terrain/draw_469_model.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg17/terrain/draw_469.inc"
};
static Sint16 draw_403_polygons[55] = {
#include "stg17/terrain/draw_403_polygons.inc"
};
static Sint32 draw_403_vertices[67] = {
#include "stg17/terrain/draw_403_vertices.inc"
};
static NJS_CNK_MODEL draw_403_model = {
#include "stg17/terrain/draw_403_model.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg17/terrain/draw_468.inc"
};
static Sint16 draw_17_polygons[19] = {
#include "stg17/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[19] = {
#include "stg17/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg17/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg17/terrain/draw_467.inc"
};
static Sint16 draw_16_polygons[191] = {
#include "stg17/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[127] = {
#include "stg17/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg17/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg17/terrain/draw_460.inc"
};
static Sint16 draw_14_polygons[89] = {
#include "stg17/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[59] = {
#include "stg17/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg17/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg17/terrain/draw_452.inc"
};
static Sint16 draw_13_polygons[19] = {
#include "stg17/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[19] = {
#include "stg17/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg17/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg17/terrain/draw_450.inc"
};
static Sint16 draw_41_polygons[19] = {
#include "stg17/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[19] = {
#include "stg17/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg17/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg17/terrain/draw_449.inc"
};
static Sint16 draw_40_polygons[43] = {
#include "stg17/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[31] = {
#include "stg17/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg17/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg17/terrain/draw_448.inc"
};
static Sint16 draw_39_polygons[81] = {
#include "stg17/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[75] = {
#include "stg17/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg17/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg17/terrain/draw_446.inc"
};
static Sint16 draw_11_polygons[217] = {
#include "stg17/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[127] = {
#include "stg17/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg17/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg17/terrain/draw_440.inc"
};
static Sint16 draw_10_polygons[265] = {
#include "stg17/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[195] = {
#include "stg17/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg17/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg17/terrain/draw_433.inc"
};
static Sint16 draw_7_polygons[151] = {
#include "stg17/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[103] = {
#include "stg17/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg17/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg17/terrain/draw_428.inc"
};
static Sint16 draw_425_polygons[43] = {
#include "stg17/terrain/draw_425_polygons.inc"
};
static Sint32 draw_425_vertices[35] = {
#include "stg17/terrain/draw_425_vertices.inc"
};
static NJS_CNK_MODEL draw_425_model = {
#include "stg17/terrain/draw_425_model.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg17/terrain/draw_425.inc"
};
static Sint16 draw_5_polygons[167] = {
#include "stg17/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[123] = {
#include "stg17/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg17/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg17/terrain/draw_424.inc"
};
static Sint16 draw_3_polygons[1413] = {
#include "stg17/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[1235] = {
#include "stg17/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg17/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg17/terrain/draw_421.inc"
};
static Sint16 draw_2_polygons[33] = {
#include "stg17/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[35] = {
#include "stg17/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg17/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg17/terrain/draw_414.inc"
};
static Sint16 draw_1_polygons[121] = {
#include "stg17/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[67] = {
#include "stg17/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg17/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg17/terrain/draw_410.inc"
};
static Sint16 draw_0_polygons[107] = {
#include "stg17/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[67] = {
#include "stg17/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg17/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg17/terrain/draw_407.inc"
};
static Sint16 draw_37_polygons[33] = {
#include "stg17/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[35] = {
#include "stg17/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg17/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg17/terrain/draw_405.inc"
};
static Sint16 draw_46_polygons[207] = {
#include "stg17/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[111] = {
#include "stg17/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg17/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg17/terrain/draw_402.inc"
};
static Sint16 draw_26_polygons[93] = {
#include "stg17/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[115] = {
#include "stg17/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg17/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg17/terrain/draw_398.inc"
};
static Sint16 draw_96_polygons[503] = {
#include "stg17/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[371] = {
#include "stg17/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg17/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg17/terrain/draw_394.inc"
};
static Sint16 draw_15_polygons[89] = {
#include "stg17/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[59] = {
#include "stg17/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg17/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg17/terrain/draw_389.inc"
};
static Sint16 draw_12_polygons[93] = {
#include "stg17/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[83] = {
#include "stg17/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg17/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg17/terrain/draw_388.inc"
};
static Sint16 draw_21_polygons[241] = {
#include "stg17/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[183] = {
#include "stg17/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg17/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg17/terrain/draw_387.inc"
};
static Sint16 draw_8_polygons[515] = {
#include "stg17/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[447] = {
#include "stg17/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg17/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg17/terrain/draw_386.inc"
};
static Sint16 draw_25_polygons[1245] = {
#include "stg17/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[1247] = {
#include "stg17/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg17/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg17/terrain/draw_385.inc"
};
static Sint16 draw_92_polygons[293] = {
#include "stg17/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[363] = {
#include "stg17/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg17/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg17/terrain/draw_384.inc"
};
static Sint16 draw_98_polygons[157] = {
#include "stg17/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[87] = {
#include "stg17/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg17/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg17/terrain/draw_383.inc"
};
static Sint16 draw_382_polygons[403] = {
#include "stg17/terrain/draw_382_polygons.inc"
};
static Sint32 draw_382_vertices[259] = {
#include "stg17/terrain/draw_382_vertices.inc"
};
static NJS_CNK_MODEL draw_382_model = {
#include "stg17/terrain/draw_382_model.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg17/terrain/draw_382.inc"
};
static Sint16 draw_86_polygons[257] = {
#include "stg17/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[147] = {
#include "stg17/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg17/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg17/terrain/draw_381.inc"
};
static Sint16 draw_84_polygons[87] = {
#include "stg17/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[67] = {
#include "stg17/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg17/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg17/terrain/draw_380.inc"
};
static Sint16 draw_83_polygons[659] = {
#include "stg17/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[507] = {
#include "stg17/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg17/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg17/terrain/draw_379.inc"
};
static Sint16 draw_82_polygons[425] = {
#include "stg17/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[295] = {
#include "stg17/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg17/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg17/terrain/draw_378.inc"
};
static Sint16 draw_93_polygons[153] = {
#include "stg17/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[139] = {
#include "stg17/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg17/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg17/terrain/draw_377.inc"
};
static Sint16 draw_91_polygons[679] = {
#include "stg17/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[519] = {
#include "stg17/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg17/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg17/terrain/draw_376.inc"
};
static Sint16 draw_127_polygons[121] = {
#include "stg17/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[75] = {
#include "stg17/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg17/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg17/terrain/draw_375.inc"
};
static Sint16 draw_113_polygons[273] = {
#include "stg17/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[219] = {
#include "stg17/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg17/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg17/terrain/draw_370.inc"
};
static Sint16 draw_328_polygons[89] = {
#include "stg17/terrain/draw_328_polygons.inc"
};
static Sint32 draw_328_vertices[99] = {
#include "stg17/terrain/draw_328_vertices.inc"
};
static NJS_CNK_MODEL draw_328_model = {
#include "stg17/terrain/draw_328_model.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg17/terrain/draw_329.inc"
};
static Sint16 draw_114_polygons[679] = {
#include "stg17/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[511] = {
#include "stg17/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg17/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg17/terrain/draw_325.inc"
};
static Sint16 draw_311_polygons[433] = {
#include "stg17/terrain/draw_311_polygons.inc"
};
static Sint32 draw_311_vertices[355] = {
#include "stg17/terrain/draw_311_vertices.inc"
};
static NJS_CNK_MODEL draw_311_model = {
#include "stg17/terrain/draw_311_model.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg17/terrain/draw_311.inc"
};
static Sint16 draw_309_polygons[705] = {
#include "stg17/terrain/draw_309_polygons.inc"
};
static Sint32 draw_309_vertices[531] = {
#include "stg17/terrain/draw_309_vertices.inc"
};
static NJS_CNK_MODEL draw_309_model = {
#include "stg17/terrain/draw_309_model.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg17/terrain/draw_310.inc"
};
static Sint16 draw_307_polygons[75] = {
#include "stg17/terrain/draw_307_polygons.inc"
};
static Sint32 draw_307_vertices[51] = {
#include "stg17/terrain/draw_307_vertices.inc"
};
static NJS_CNK_MODEL draw_307_model = {
#include "stg17/terrain/draw_307_model.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg17/terrain/draw_308.inc"
};
static Sint16 draw_305_polygons[705] = {
#include "stg17/terrain/draw_305_polygons.inc"
};
static Sint32 draw_305_vertices[531] = {
#include "stg17/terrain/draw_305_vertices.inc"
};
static NJS_CNK_MODEL draw_305_model = {
#include "stg17/terrain/draw_305_model.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg17/terrain/draw_306.inc"
};
static Sint16 draw_304_polygons[153] = {
#include "stg17/terrain/draw_304_polygons.inc"
};
static Sint32 draw_304_vertices[107] = {
#include "stg17/terrain/draw_304_vertices.inc"
};
static NJS_CNK_MODEL draw_304_model = {
#include "stg17/terrain/draw_304_model.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg17/terrain/draw_304.inc"
};
static Sint16 draw_303_polygons[61] = {
#include "stg17/terrain/draw_303_polygons.inc"
};
static Sint32 draw_303_vertices[75] = {
#include "stg17/terrain/draw_303_vertices.inc"
};
static NJS_CNK_MODEL draw_303_model = {
#include "stg17/terrain/draw_303_model.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg17/terrain/draw_303.inc"
};
static Sint16 draw_110_polygons[171] = {
#include "stg17/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[107] = {
#include "stg17/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg17/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg17/terrain/draw_302.inc"
};
static Sint16 draw_293_polygons[513] = {
#include "stg17/terrain/draw_293_polygons.inc"
};
static Sint32 draw_293_vertices[351] = {
#include "stg17/terrain/draw_293_vertices.inc"
};
static NJS_CNK_MODEL draw_293_model = {
#include "stg17/terrain/draw_293_model.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg17/terrain/draw_293.inc"
};
static Sint16 draw_108_polygons[249] = {
#include "stg17/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[155] = {
#include "stg17/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg17/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg17/terrain/draw_292.inc"
};
static Sint16 draw_285_polygons[505] = {
#include "stg17/terrain/draw_285_polygons.inc"
};
static Sint32 draw_285_vertices[327] = {
#include "stg17/terrain/draw_285_vertices.inc"
};
static NJS_CNK_MODEL draw_285_model = {
#include "stg17/terrain/draw_285_model.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg17/terrain/draw_285.inc"
};
static Sint16 draw_81_polygons[319] = {
#include "stg17/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[163] = {
#include "stg17/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg17/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg17/terrain/draw_284.inc"
};
static Sint16 draw_100_polygons[121] = {
#include "stg17/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[75] = {
#include "stg17/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg17/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg17/terrain/draw_273.inc"
};
static Sint16 draw_272_polygons[121] = {
#include "stg17/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[75] = {
#include "stg17/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg17/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg17/terrain/draw_272.inc"
};
static Sint16 draw_99_polygons[33] = {
#include "stg17/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[35] = {
#include "stg17/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg17/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg17/terrain/draw_271.inc"
};
static Sint16 draw_263_polygons[631] = {
#include "stg17/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[571] = {
#include "stg17/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg17/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg17/terrain/draw_264.inc"
};
static Sint16 draw_251_polygons[97] = {
#include "stg17/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[63] = {
#include "stg17/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg17/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg17/terrain/draw_262.inc"
};
static Sint16 draw_248_polygons[425] = {
#include "stg17/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[515] = {
#include "stg17/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg17/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg17/terrain/draw_250.inc"
};
static Sint16 draw_175_polygons[187] = {
#include "stg17/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[203] = {
#include "stg17/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg17/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg17/terrain/draw_247.inc"
};
static Sint16 draw_241_polygons[753] = {
#include "stg17/terrain/draw_241_polygons.inc"
};
static Sint32 draw_241_vertices[495] = {
#include "stg17/terrain/draw_241_vertices.inc"
};
static NJS_CNK_MODEL draw_241_model = {
#include "stg17/terrain/draw_241_model.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg17/terrain/draw_243.inc"
};
static Sint16 draw_240_polygons[215] = {
#include "stg17/terrain/draw_240_polygons.inc"
};
static Sint32 draw_240_vertices[103] = {
#include "stg17/terrain/draw_240_vertices.inc"
};
static NJS_CNK_MODEL draw_240_model = {
#include "stg17/terrain/draw_240_model.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg17/terrain/draw_240.inc"
};
static Sint16 draw_239_polygons[151] = {
#include "stg17/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[99] = {
#include "stg17/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg17/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg17/terrain/draw_239.inc"
};
static Sint16 draw_236_polygons[429] = {
#include "stg17/terrain/draw_236_polygons.inc"
};
static Sint32 draw_236_vertices[515] = {
#include "stg17/terrain/draw_236_vertices.inc"
};
static NJS_CNK_MODEL draw_236_model = {
#include "stg17/terrain/draw_236_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg17/terrain/draw_238.inc"
};
static Sint16 draw_166_polygons[233] = {
#include "stg17/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[251] = {
#include "stg17/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg17/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg17/terrain/draw_235.inc"
};
static Sint16 draw_229_polygons[753] = {
#include "stg17/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[495] = {
#include "stg17/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg17/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg17/terrain/draw_231.inc"
};
static Sint16 draw_165_polygons[19] = {
#include "stg17/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[19] = {
#include "stg17/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg17/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg17/terrain/draw_228.inc"
};
static Sint16 draw_222_polygons[333] = {
#include "stg17/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[347] = {
#include "stg17/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg17/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg17/terrain/draw_227.inc"
};
static Sint16 draw_225_polygons[1885] = {
#include "stg17/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[1539] = {
#include "stg17/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg17/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg17/terrain/draw_225.inc"
};
static Sint16 draw_218_polygons[95] = {
#include "stg17/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[91] = {
#include "stg17/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg17/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg17/terrain/draw_221.inc"
};
static Sint16 draw_212_polygons[333] = {
#include "stg17/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[347] = {
#include "stg17/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg17/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg17/terrain/draw_217.inc"
};
static Sint16 draw_193_polygons[821] = {
#include "stg17/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[587] = {
#include "stg17/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg17/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg17/terrain/draw_193.inc"
};
static Sint16 draw_190_polygons[49] = {
#include "stg17/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[51] = {
#include "stg17/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg17/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg17/terrain/draw_192.inc"
};
static Sint16 draw_185_polygons[57] = {
#include "stg17/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[43] = {
#include "stg17/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg17/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg17/terrain/draw_189.inc"
};
static Sint16 draw_184_polygons[663] = {
#include "stg17/terrain/draw_184_polygons.inc"
};
static Sint32 draw_184_vertices[555] = {
#include "stg17/terrain/draw_184_vertices.inc"
};
static NJS_CNK_MODEL draw_184_model = {
#include "stg17/terrain/draw_184_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg17/terrain/draw_184.inc"
};
static Sint16 draw_151_polygons[263] = {
#include "stg17/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[203] = {
#include "stg17/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg17/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg17/terrain/draw_151.inc"
};
static Sint16 draw_144_polygons[245] = {
#include "stg17/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[159] = {
#include "stg17/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg17/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg17/terrain/draw_144.inc"
};
static Sint16 draw_139_polygons[121] = {
#include "stg17/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[91] = {
#include "stg17/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg17/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg17/terrain/draw_139.inc"
};
static Sint16 draw_134_polygons[121] = {
#include "stg17/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[75] = {
#include "stg17/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg17/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg17/terrain/draw_134.inc"
};
static Sint16 draw_129_polygons[83] = {
#include "stg17/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[67] = {
#include "stg17/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg17/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg17/terrain/draw_132.inc"
};
static Sint16 draw_97_polygons[451] = {
#include "stg17/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[351] = {
#include "stg17/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg17/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg17/terrain/draw_126.inc"
};
static Sint16 draw_122_polygons[505] = {
#include "stg17/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[327] = {
#include "stg17/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg17/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg17/terrain/draw_122.inc"
};
static Sint16 draw_95_polygons[89] = {
#include "stg17/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[67] = {
#include "stg17/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg17/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg17/terrain/draw_121.inc"
};
static Sint16 draw_112_polygons[513] = {
#include "stg17/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[351] = {
#include "stg17/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg17/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg17/terrain/draw_112.inc"
};
static Sint16 draw_94_polygons[19] = {
#include "stg17/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[19] = {
#include "stg17/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg17/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg17/terrain/draw_94.inc"
};
static Sint16 draw_90_polygons[213] = {
#include "stg17/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[151] = {
#include "stg17/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg17/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg17/terrain/draw_90.inc"
};
static Sint16 draw_89_polygons[403] = {
#include "stg17/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[259] = {
#include "stg17/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg17/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg17/terrain/draw_89.inc"
};
static Sint16 draw_88_polygons[301] = {
#include "stg17/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[255] = {
#include "stg17/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg17/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg17/terrain/draw_88.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg17/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg17/terrain/collision_3.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg17/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg17/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg17/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg17/terrain/collision_8.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg17/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg17/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg17/terrain/collision_14.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg17/terrain/collision_18.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg17/terrain/collision_31.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg17/terrain/collision_33.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg17/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg17/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg17/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg17/terrain/collision_45.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg17/terrain/collision_49.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg17/terrain/collision_51.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg17/terrain/collision_53.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg17/terrain/collision_55.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg17/terrain/collision_57.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg17/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg17/terrain/collision_59.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg17/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg17/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg17/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg17/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg17/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg17/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg17/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg17/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg17/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg17/terrain/collision_76.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg17/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg17/terrain/collision_80.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg17/terrain/collision_83.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg17/terrain/collision_98.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg17/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg17/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg17/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg17/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg17/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg17/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg17/terrain/collision_106.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg17/terrain/collision_107.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg17/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg17/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg17/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg17/terrain/collision_111.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg17/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg17/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg17/terrain/collision_115.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg17/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg17/terrain/collision_128.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg17/terrain/collision_130.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg17/terrain/collision_149.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg17/terrain/collision_150.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg17/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg17/terrain/collision_152.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg17/terrain/collision_155.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg17/terrain/collision_156.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg17/terrain/collision_157.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg17/terrain/collision_158.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg17/terrain/collision_159.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg17/terrain/collision_160.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg17/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg17/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg17/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg17/terrain/collision_164.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg17/terrain/collision_167.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg17/terrain/collision_169.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg17/terrain/collision_171.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg17/terrain/collision_179.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg17/terrain/collision_181.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg17/terrain/collision_182.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg17/terrain/collision_183.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg17/terrain/collision_186.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg17/terrain/collision_187.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg17/terrain/collision_188.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg17/terrain/collision_189.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg17/terrain/collision_190.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg17/terrain/draw_476.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg17/terrain/draw_475.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg17/terrain/draw_474.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg17/terrain/draw_473.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg17/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg17/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg17/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg17/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg17/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg17/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg17/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg17/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg17/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg17/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg17/terrain/draw_456.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg17/terrain/draw_455.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg17/terrain/draw_454.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg17/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg17/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg17/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg17/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg17/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg17/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg17/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg17/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg17/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg17/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg17/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg17/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg17/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg17/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg17/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg17/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg17/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg17/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg17/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg17/terrain/draw_426.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg17/terrain/draw_423.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg17/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg17/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg17/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg17/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg17/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg17/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg17/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg17/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg17/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg17/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg17/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg17/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg17/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg17/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg17/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg17/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg17/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg17/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg17/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg17/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg17/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg17/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg17/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg17/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg17/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg17/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg17/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg17/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg17/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg17/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg17/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg17/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg17/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg17/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg17/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg17/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg17/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg17/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg17/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg17/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg17/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg17/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg17/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg17/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg17/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg17/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg17/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg17/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg17/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg17/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg17/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg17/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg17/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg17/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg17/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg17/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg17/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg17/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg17/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg17/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg17/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg17/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg17/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg17/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg17/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg17/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg17/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg17/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg17/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg17/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg17/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg17/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg17/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg17/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg17/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg17/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg17/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg17/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg17/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg17/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg17/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg17/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg17/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg17/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg17/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg17/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg17/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg17/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg17/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg17/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg17/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg17/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg17/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg17/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg17/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg17/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg17/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg17/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg17/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg17/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg17/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg17/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg17/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg17/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg17/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg17/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg17/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg17/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg17/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg17/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg17/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg17/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg17/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg17/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg17/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg17/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg17/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg17/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg17/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg17/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg17/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg17/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg17/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg17/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg17/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg17/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg17/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg17/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg17/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg17/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg17/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg17/terrain/draw_248.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg17/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg17/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg17/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg17/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg17/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg17/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg17/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg17/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg17/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg17/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg17/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg17/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg17/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg17/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg17/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg17/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg17/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg17/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg17/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg17/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg17/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg17/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg17/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg17/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg17/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg17/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg17/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg17/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg17/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg17/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg17/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg17/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg17/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg17/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg17/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg17/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg17/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg17/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg17/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg17/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg17/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg17/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg17/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg17/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg17/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg17/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg17/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg17/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg17/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg17/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg17/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg17/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg17/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg17/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg17/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg17/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg17/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg17/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg17/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg17/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg17/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg17/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg17/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg17/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg17/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg17/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg17/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg17/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg17/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg17/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg17/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg17/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg17/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg17/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg17/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg17/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg17/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg17/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg17/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg17/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg17/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg17/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg17/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg17/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg17/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg17/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg17/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg17/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg17/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg17/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg17/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg17/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg17/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg17/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg17/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg17/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg17/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg17/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg17/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg17/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg17/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg17/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg17/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg17/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg17/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg17/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg17/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg17/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg17/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg17/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg17/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg17/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg17/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg17/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg17/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg17/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg17/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg17/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg17/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg17/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg17/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg17/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg17/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg17/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg17/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg17/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg17/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg17/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg17/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg17/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg17/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg17/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg17/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg17/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg17/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg17/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg17/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg17/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg17/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg17/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg17/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg17/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg17/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg17/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg17/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg17/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg17/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg17/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg17/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg17/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg17/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg17/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg17/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg17/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg17/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg17/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg17/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg17/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg17/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg17/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg17/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg17/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg17/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg17/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg17/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg17/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg17/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg17/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg17/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg17/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg17/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg17/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg17/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg17/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg17/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg17/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg17/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg17/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg17/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg17/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg17/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg17/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg17/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg17/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg17/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg17/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg17/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg17/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg17/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg17/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg17/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg17/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg17/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg17/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg17/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg17/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg17/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg17/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg17/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg17/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg17/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg17/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg17/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg17/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg17/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg17/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg17/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg17/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg17/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg17/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg17/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg17/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg17/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg17/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg17/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg17/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg17/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg17/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg17/terrain/draw_0.inc"
};
