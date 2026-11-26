#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg31/green_hill.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char green_hill_land_filenames[195] = {
#include "stg31/terrain/green_hill_land_filenames.inc"
};

/* STG31.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between water_land.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[419];
struct land_table green_hill_land;
static NJS_TEXNAME textures[12];
NJS_TEXLIST green_hill_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[123];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[69];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[45];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[23];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[7];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[6];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[21];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[11];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[7];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[6];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[7];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[6];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[7];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[6];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[24];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[12];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[28];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[15];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[9];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[8];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[7];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[6];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[44];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[20];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[17];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[10];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[89];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[39];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[6];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[5];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[32];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[15];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[21];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[14];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[45];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[20];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[22];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[12];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[32];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[16];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[32];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[16];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[9];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[8];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[35];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[16];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[48];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[21];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[35];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[16];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[35];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[16];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[26];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[18];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[27];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[17];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[39];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[24];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[30];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[16];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[44];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[20];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[45];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[20];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[44];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[20];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[9];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[8];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[9];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[8];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[11];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[10];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[18];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[12];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[18];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[12];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[54];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[25];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[22];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[15];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[22];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[15];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[58];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[30];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[15];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[8];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[18];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[9];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[17];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[16];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[23];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[14];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[23];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[14];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[55];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[27];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[28];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[15];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[98];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[43];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[40];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[22];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[98];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[43];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[32];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[16];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[15];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[8];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[55];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[30];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[23];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[15];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[79];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[36];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[36];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[16];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[107];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[44];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[43];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[25];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_114_materials[1];
static Sint16 collision_114_faces[94];
static NJS_MESHSET collision_114_meshsets[1];
static NJS_POINT3 collision_114_points[54];
static NJS_MODEL collision_114_model;
static NJS_OBJECT collision_114;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[61];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[30];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[54];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[29];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[37];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[24];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[49];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[27];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[137];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[68];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[102];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[56];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[65];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[36];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[71];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[34];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[36];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[16];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[19];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[8];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[36];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[22];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[82];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[45];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_145_materials[1];
static Sint16 collision_145_faces[24];
static NJS_MESHSET collision_145_meshsets[1];
static NJS_POINT3 collision_145_points[16];
static NJS_MODEL collision_145_model;
static NJS_OBJECT collision_145;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[24];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[16];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_147_materials[1];
static Sint16 collision_147_faces[40];
static NJS_MESHSET collision_147_meshsets[1];
static NJS_POINT3 collision_147_points[26];
static NJS_MODEL collision_147_model;
static NJS_OBJECT collision_147;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[101];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[57];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[45];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[28];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[62];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[38];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[82];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[38];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[48];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[28];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[32];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[24];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[120];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[68];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[59];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[36];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[173];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[96];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_157_materials[1];
static Sint16 collision_157_faces[109];
static NJS_MESHSET collision_157_meshsets[1];
static NJS_POINT3 collision_157_points[60];
static NJS_MODEL collision_157_model;
static NJS_OBJECT collision_157;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[44];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[34];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[90];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[46];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[65];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[37];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[87];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[44];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[18];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[16];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[28];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[15];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_MATERIAL collision_164_materials[1];
static Sint16 collision_164_faces[127];
static NJS_MESHSET collision_164_meshsets[1];
static NJS_POINT3 collision_164_points[69];
static NJS_MODEL collision_164_model;
static NJS_OBJECT collision_164;
static NJS_MATERIAL collision_165_materials[1];
static Sint16 collision_165_faces[92];
static NJS_MESHSET collision_165_meshsets[1];
static NJS_POINT3 collision_165_points[49];
static NJS_MODEL collision_165_model;
static NJS_OBJECT collision_165;
static NJS_MATERIAL collision_166_materials[1];
static Sint16 collision_166_faces[63];
static NJS_MESHSET collision_166_meshsets[1];
static NJS_POINT3 collision_166_points[44];
static NJS_MODEL collision_166_model;
static NJS_OBJECT collision_166;
static NJS_MATERIAL collision_167_materials[1];
static Sint16 collision_167_faces[44];
static NJS_MESHSET collision_167_meshsets[1];
static NJS_POINT3 collision_167_points[25];
static NJS_MODEL collision_167_model;
static NJS_OBJECT collision_167;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[33];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[20];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[27];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[18];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_170_materials[1];
static Sint16 collision_170_faces[7];
static NJS_MESHSET collision_170_meshsets[1];
static NJS_POINT3 collision_170_points[6];
static NJS_MODEL collision_170_model;
static NJS_OBJECT collision_170;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[12];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[8];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_172_materials[1];
static Sint16 collision_172_faces[13];
static NJS_MESHSET collision_172_meshsets[1];
static NJS_POINT3 collision_172_points[9];
static NJS_MODEL collision_172_model;
static NJS_OBJECT collision_172;
static NJS_MATERIAL collision_173_materials[1];
static Sint16 collision_173_faces[9];
static NJS_MESHSET collision_173_meshsets[1];
static NJS_POINT3 collision_173_points[8];
static NJS_MODEL collision_173_model;
static NJS_OBJECT collision_173;
static NJS_MATERIAL collision_174_materials[1];
static Sint16 collision_174_faces[50];
static NJS_MESHSET collision_174_meshsets[1];
static NJS_POINT3 collision_174_points[18];
static NJS_MODEL collision_174_model;
static NJS_OBJECT collision_174;
static NJS_MATERIAL collision_175_materials[1];
static Sint16 collision_175_faces[284];
static NJS_MESHSET collision_175_meshsets[1];
static NJS_POINT3 collision_175_points[108];
static NJS_MODEL collision_175_model;
static NJS_OBJECT collision_175;
static NJS_MATERIAL collision_176_materials[1];
static Sint16 collision_176_faces[26];
static NJS_MESHSET collision_176_meshsets[1];
static NJS_POINT3 collision_176_points[24];
static NJS_MODEL collision_176_model;
static NJS_OBJECT collision_176;
static NJS_MATERIAL collision_177_materials[1];
static Sint16 collision_177_faces[7];
static NJS_MESHSET collision_177_meshsets[1];
static NJS_POINT3 collision_177_points[6];
static NJS_MODEL collision_177_model;
static NJS_OBJECT collision_177;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[62];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[25];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static Sint16 draw_234_polygons[321];
static Sint32 draw_234_vertices[187];
static NJS_CNK_MODEL draw_234_model;
static NJS_CNK_OBJECT draw_237;
static Sint16 draw_231_polygons[137];
static Sint32 draw_231_vertices[107];
static NJS_CNK_MODEL draw_231_model;
static NJS_CNK_OBJECT draw_233;
static Sint16 draw_230_polygons[321];
static Sint32 draw_230_vertices[187];
static NJS_CNK_MODEL draw_230_model;
static NJS_CNK_OBJECT draw_230;
static Sint16 draw_229_polygons[137];
static Sint32 draw_229_vertices[107];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_228_polygons[103];
static Sint32 draw_228_vertices[75];
static NJS_CNK_MODEL draw_228_model;
static NJS_CNK_OBJECT draw_228;
static Sint16 draw_226_polygons[123];
static Sint32 draw_226_vertices[83];
static NJS_CNK_MODEL draw_226_model;
static NJS_CNK_OBJECT draw_227;
static Sint16 draw_225_polygons[123];
static Sint32 draw_225_vertices[83];
static NJS_CNK_MODEL draw_225_model;
static NJS_CNK_OBJECT draw_225;
static Sint16 draw_224_polygons[325];
static Sint32 draw_224_vertices[199];
static NJS_CNK_MODEL draw_224_model;
static NJS_CNK_OBJECT draw_224;
static Sint16 draw_223_polygons[253];
static Sint32 draw_223_vertices[143];
static NJS_CNK_MODEL draw_223_model;
static NJS_CNK_OBJECT draw_223;
static Sint16 draw_222_polygons[141];
static Sint32 draw_222_vertices[107];
static NJS_CNK_MODEL draw_222_model;
static NJS_CNK_OBJECT draw_222;
static Sint16 draw_221_polygons[141];
static Sint32 draw_221_vertices[107];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_221;
static Sint16 draw_220_polygons[239];
static Sint32 draw_220_vertices[139];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_220;
static Sint16 draw_219_polygons[191];
static Sint32 draw_219_vertices[131];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_218_polygons[227];
static Sint32 draw_218_vertices[135];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_218;
static Sint16 draw_199_polygons[469];
static Sint32 draw_199_vertices[275];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_198_polygons[213];
static Sint32 draw_198_vertices[115];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_197_polygons[325];
static Sint32 draw_197_vertices[187];
static NJS_CNK_MODEL draw_197_model;
static NJS_CNK_OBJECT draw_197;
static Sint16 draw_196_polygons[141];
static Sint32 draw_196_vertices[107];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_195_polygons[345];
static Sint32 draw_195_vertices[199];
static NJS_CNK_MODEL draw_195_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_193_polygons[325];
static Sint32 draw_193_vertices[199];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_192_polygons[141];
static Sint32 draw_192_vertices[107];
static NJS_CNK_MODEL draw_192_model;
static NJS_CNK_OBJECT draw_192;
static Sint16 draw_191_polygons[157];
static Sint32 draw_191_vertices[111];
static NJS_CNK_MODEL draw_191_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_190_polygons[225];
static Sint32 draw_190_vertices[167];
static NJS_CNK_MODEL draw_190_model;
static NJS_CNK_OBJECT draw_190;
static Sint16 draw_189_polygons[169];
static Sint32 draw_189_vertices[107];
static NJS_CNK_MODEL draw_189_model;
static NJS_CNK_OBJECT draw_189;
static Sint16 draw_183_polygons[333];
static Sint32 draw_183_vertices[195];
static NJS_CNK_MODEL draw_183_model;
static NJS_CNK_OBJECT draw_188;
static Sint16 draw_181_polygons[427];
static Sint32 draw_181_vertices[235];
static NJS_CNK_MODEL draw_181_model;
static NJS_CNK_OBJECT draw_182;
static Sint16 draw_179_polygons[121];
static Sint32 draw_179_vertices[131];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_178_polygons[85];
static Sint32 draw_178_vertices[47];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_176_polygons[135];
static Sint32 draw_176_vertices[99];
static NJS_CNK_MODEL draw_176_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_175_polygons[81];
static Sint32 draw_175_vertices[71];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_175;
static Sint16 draw_174_polygons[505];
static Sint32 draw_174_vertices[391];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_172_polygons[167];
static Sint32 draw_172_vertices[119];
static NJS_CNK_MODEL draw_172_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_166_polygons[537];
static Sint32 draw_166_vertices[367];
static NJS_CNK_MODEL draw_166_model;
static NJS_CNK_OBJECT draw_171;
static Sint16 draw_165_polygons[131];
static Sint32 draw_165_vertices[123];
static NJS_CNK_MODEL draw_165_model;
static NJS_CNK_OBJECT draw_165;
static Sint16 draw_164_polygons[93];
static Sint32 draw_164_vertices[67];
static NJS_CNK_MODEL draw_164_model;
static NJS_CNK_OBJECT draw_164;
static Sint16 draw_161_polygons[141];
static Sint32 draw_161_vertices[107];
static NJS_CNK_MODEL draw_161_model;
static NJS_CNK_OBJECT draw_163;
static Sint16 draw_160_polygons[247];
static Sint32 draw_160_vertices[163];
static NJS_CNK_MODEL draw_160_model;
static NJS_CNK_OBJECT draw_160;
static Sint16 draw_159_polygons[141];
static Sint32 draw_159_vertices[107];
static NJS_CNK_MODEL draw_159_model;
static NJS_CNK_OBJECT draw_159;
static Sint16 draw_158_polygons[105];
static Sint32 draw_158_vertices[67];
static NJS_CNK_MODEL draw_158_model;
static NJS_CNK_OBJECT draw_158;
static Sint16 draw_157_polygons[469];
static Sint32 draw_157_vertices[275];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_157;
static Sint16 draw_156_polygons[349];
static Sint32 draw_156_vertices[235];
static NJS_CNK_MODEL draw_156_model;
static NJS_CNK_OBJECT draw_156;
static Sint16 draw_155_polygons[441];
static Sint32 draw_155_vertices[243];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_154_polygons[471];
static Sint32 draw_154_vertices[275];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_153_polygons[443];
static Sint32 draw_153_vertices[235];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_152_polygons[105];
static Sint32 draw_152_vertices[67];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_151_polygons[105];
static Sint32 draw_151_vertices[67];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_150_polygons[325];
static Sint32 draw_150_vertices[191];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_148_polygons[121];
static Sint32 draw_148_vertices[127];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_147_polygons[195];
static Sint32 draw_147_vertices[119];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_146_polygons[2169];
static Sint32 draw_146_vertices[1779];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_145_polygons[63];
static Sint32 draw_145_vertices[47];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_144_polygons[29];
static Sint32 draw_144_vertices[27];
static NJS_CNK_MODEL draw_144_model;
static NJS_CNK_OBJECT draw_144;
static Sint16 draw_143_polygons[167];
static Sint32 draw_143_vertices[119];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_140_polygons[681];
static Sint32 draw_140_vertices[471];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_138_polygons[463];
static Sint32 draw_138_vertices[299];
static NJS_CNK_MODEL draw_138_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_137_polygons[553];
static Sint32 draw_137_vertices[527];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_137;
static Sint16 draw_136_polygons[233];
static Sint32 draw_136_vertices[207];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_134_polygons[217];
static Sint32 draw_134_vertices[199];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_131_polygons[393];
static Sint32 draw_131_vertices[387];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_130_polygons[83];
static Sint32 draw_130_vertices[75];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[171];
static Sint32 draw_129_vertices[163];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[137];
static Sint32 draw_128_vertices[107];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_126_polygons[393];
static Sint32 draw_126_vertices[387];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_125_polygons[97];
static Sint32 draw_125_vertices[95];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_124_polygons[317];
static Sint32 draw_124_vertices[187];
static NJS_CNK_MODEL draw_124_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_123_polygons[317];
static Sint32 draw_123_vertices[187];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_123;
static Sint16 draw_117_polygons[105];
static Sint32 draw_117_vertices[67];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_116_polygons[329];
static Sint32 draw_116_vertices[323];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[205];
static Sint32 draw_115_vertices[159];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[413];
static Sint32 draw_114_vertices[391];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_112_polygons[233];
static Sint32 draw_112_vertices[131];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_110_polygons[217];
static Sint32 draw_110_vertices[219];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_109_polygons[217];
static Sint32 draw_109_vertices[199];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_108_polygons[463];
static Sint32 draw_108_vertices[267];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_106_polygons[413];
static Sint32 draw_106_vertices[375];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_105_polygons[727];
static Sint32 draw_105_vertices[415];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[439];
static Sint32 draw_104_vertices[475];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[265];
static Sint32 draw_103_vertices[191];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[121];
static Sint32 draw_102_vertices[131];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[193];
static Sint32 draw_101_vertices[171];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[487];
static Sint32 draw_100_vertices[399];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_98_polygons[109];
static Sint32 draw_98_vertices[87];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_97_polygons[35];
static Sint32 draw_97_vertices[35];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[197];
static Sint32 draw_96_vertices[123];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[321];
static Sint32 draw_95_vertices[311];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[175];
static Sint32 draw_94_vertices[111];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[439];
static Sint32 draw_93_vertices[363];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[421];
static Sint32 draw_92_vertices[267];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_89_polygons[2169];
static Sint32 draw_89_vertices[1795];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_88_polygons[101];
static Sint32 draw_88_vertices[67];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_87_polygons[113];
static Sint32 draw_87_vertices[95];
static NJS_CNK_MODEL draw_87_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_86_polygons[95];
static Sint32 draw_86_vertices[75];
static NJS_CNK_MODEL draw_86_model;
static NJS_CNK_OBJECT draw_86;
static Sint16 draw_85_polygons[135];
static Sint32 draw_85_vertices[127];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_85;
static Sint16 draw_84_polygons[105];
static Sint32 draw_84_vertices[95];
static NJS_CNK_MODEL draw_84_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_83_polygons[285];
static Sint32 draw_83_vertices[259];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_82_polygons[177];
static Sint32 draw_82_vertices[143];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_82;
static Sint16 draw_81_polygons[123];
static Sint32 draw_81_vertices[115];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_80_polygons[119];
static Sint32 draw_80_vertices[87];
static NJS_CNK_MODEL draw_80_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_79_polygons[155];
static Sint32 draw_79_vertices[159];
static NJS_CNK_MODEL draw_79_model;
static NJS_CNK_OBJECT draw_79;
static Sint16 draw_78_polygons[151];
static Sint32 draw_78_vertices[139];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_78;
static Sint16 draw_76_polygons[49];
static Sint32 draw_76_vertices[51];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_75_polygons[49];
static Sint32 draw_75_vertices[51];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_74_polygons[95];
static Sint32 draw_74_vertices[75];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_73_polygons[289];
static Sint32 draw_73_vertices[227];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[179];
static Sint32 draw_72_vertices[191];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_71_polygons[285];
static Sint32 draw_71_vertices[199];
static NJS_CNK_MODEL draw_71_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_70_polygons[215];
static Sint32 draw_70_vertices[247];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_70;
static Sint16 draw_69_polygons[63];
static Sint32 draw_69_vertices[51];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[119];
static Sint32 draw_68_vertices[99];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[103];
static Sint32 draw_67_vertices[99];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[139];
static Sint32 draw_65_vertices[139];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_64_polygons[157];
static Sint32 draw_64_vertices[163];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[125];
static Sint32 draw_63_vertices[99];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[135];
static Sint32 draw_62_vertices[111];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[49];
static Sint32 draw_61_vertices[51];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[77];
static Sint32 draw_60_vertices[75];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[121];
static Sint32 draw_59_vertices[111];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[81];
static Sint32 draw_58_vertices[75];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[277];
static Sint32 draw_57_vertices[235];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[137];
static Sint32 draw_56_vertices[99];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[107];
static Sint32 draw_55_vertices[91];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[87];
static Sint32 draw_54_vertices[75];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[75];
static Sint32 draw_53_vertices[63];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[125];
static Sint32 draw_52_vertices[103];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[43];
static Sint32 draw_51_vertices[35];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[253];
static Sint32 draw_50_vertices[143];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_39_polygons[119];
static Sint32 draw_39_vertices[123];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_31_polygons[127];
static Sint32 draw_31_vertices[127];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_30_polygons[1011];
static Sint32 draw_30_vertices[875];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[207];
static Sint32 draw_29_vertices[207];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[979];
static Sint32 draw_28_vertices[607];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[139];
static Sint32 draw_27_vertices[131];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[321];
static Sint32 draw_26_vertices[187];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[359];
static Sint32 draw_25_vertices[347];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[541];
static Sint32 draw_24_vertices[511];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[471];
static Sint32 draw_23_vertices[471];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[91];
static Sint32 draw_22_vertices[87];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[321];
static Sint32 draw_21_vertices[323];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[661];
static Sint32 draw_20_vertices[591];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[517];
static Sint32 draw_19_vertices[323];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[1555];
static Sint32 draw_18_vertices[1443];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[1431];
static Sint32 draw_17_vertices[1423];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[505];
static Sint32 draw_16_vertices[331];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[213];
static Sint32 draw_15_vertices[115];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[1231];
static Sint32 draw_14_vertices[847];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[1215];
static Sint32 draw_13_vertices[1199];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[635];
static Sint32 draw_12_vertices[383];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[505];
static Sint32 draw_11_vertices[411];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[535];
static Sint32 draw_10_vertices[355];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[151];
static Sint32 draw_9_vertices[139];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[119];
static Sint32 draw_8_vertices[99];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[49];
static Sint32 draw_7_vertices[39];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[105];
static Sint32 draw_6_vertices[99];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[103];
static Sint32 draw_5_vertices[75];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_2_polygons[293];
static Sint32 draw_2_vertices[227];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_0_polygons[491];
static Sint32 draw_0_vertices[255];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
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
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_231;
static NJS_CNK_OBJECT draw_226;
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
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_167;
static NJS_CNK_OBJECT draw_166;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_98;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_65;
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
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[419] = {
#include "stg31/terrain/entries.inc"
};
struct land_table green_hill_land = {
#include "stg31/terrain/green_hill_land.inc"
};
static NJS_TEXNAME textures[12] = {
#include "stg31/terrain/textures.inc"
};
NJS_TEXLIST green_hill_land_texlist = {
#include "stg31/terrain/green_hill_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg31/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[123] = {
#include "stg31/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg31/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[69] = {
#include "stg31/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg31/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg31/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg31/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[45] = {
#include "stg31/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg31/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[23] = {
#include "stg31/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg31/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg31/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg31/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[7] = {
#include "stg31/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg31/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[6] = {
#include "stg31/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg31/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg31/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg31/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[21] = {
#include "stg31/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg31/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[11] = {
#include "stg31/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg31/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg31/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg31/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[7] = {
#include "stg31/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg31/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[6] = {
#include "stg31/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg31/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg31/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg31/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[7] = {
#include "stg31/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg31/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[6] = {
#include "stg31/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg31/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg31/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg31/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[7] = {
#include "stg31/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg31/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[6] = {
#include "stg31/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg31/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg31/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg31/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[24] = {
#include "stg31/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg31/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[12] = {
#include "stg31/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg31/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg31/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg31/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[28] = {
#include "stg31/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg31/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[15] = {
#include "stg31/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg31/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg31/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg31/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[9] = {
#include "stg31/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg31/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[8] = {
#include "stg31/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg31/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg31/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg31/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[7] = {
#include "stg31/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg31/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[6] = {
#include "stg31/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg31/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg31/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg31/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[44] = {
#include "stg31/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg31/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[20] = {
#include "stg31/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg31/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg31/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg31/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[17] = {
#include "stg31/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg31/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[10] = {
#include "stg31/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg31/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg31/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg31/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[89] = {
#include "stg31/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg31/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[39] = {
#include "stg31/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg31/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg31/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg31/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[6] = {
#include "stg31/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg31/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[5] = {
#include "stg31/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg31/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg31/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg31/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[32] = {
#include "stg31/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg31/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[15] = {
#include "stg31/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg31/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg31/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg31/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[21] = {
#include "stg31/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg31/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[14] = {
#include "stg31/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg31/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg31/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg31/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[45] = {
#include "stg31/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg31/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[20] = {
#include "stg31/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg31/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg31/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg31/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[22] = {
#include "stg31/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg31/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[12] = {
#include "stg31/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg31/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg31/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg31/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[32] = {
#include "stg31/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg31/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[16] = {
#include "stg31/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg31/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg31/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg31/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[32] = {
#include "stg31/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg31/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[16] = {
#include "stg31/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg31/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg31/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg31/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[9] = {
#include "stg31/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg31/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[8] = {
#include "stg31/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg31/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg31/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg31/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[35] = {
#include "stg31/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg31/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[16] = {
#include "stg31/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg31/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg31/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg31/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[48] = {
#include "stg31/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg31/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[21] = {
#include "stg31/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg31/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg31/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg31/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[35] = {
#include "stg31/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg31/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[16] = {
#include "stg31/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg31/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg31/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg31/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[35] = {
#include "stg31/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg31/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[16] = {
#include "stg31/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg31/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg31/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg31/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[26] = {
#include "stg31/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg31/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[18] = {
#include "stg31/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg31/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg31/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg31/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[27] = {
#include "stg31/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg31/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[17] = {
#include "stg31/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg31/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg31/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg31/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[39] = {
#include "stg31/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg31/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[24] = {
#include "stg31/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg31/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg31/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg31/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[30] = {
#include "stg31/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg31/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[16] = {
#include "stg31/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg31/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg31/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg31/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[44] = {
#include "stg31/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg31/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[20] = {
#include "stg31/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg31/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg31/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg31/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[45] = {
#include "stg31/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg31/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[20] = {
#include "stg31/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg31/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg31/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg31/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[44] = {
#include "stg31/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg31/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[20] = {
#include "stg31/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg31/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg31/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg31/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[9] = {
#include "stg31/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg31/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[8] = {
#include "stg31/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg31/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg31/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg31/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[9] = {
#include "stg31/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg31/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[8] = {
#include "stg31/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg31/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg31/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg31/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[11] = {
#include "stg31/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg31/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[10] = {
#include "stg31/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg31/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg31/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg31/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[18] = {
#include "stg31/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg31/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[12] = {
#include "stg31/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg31/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg31/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg31/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[18] = {
#include "stg31/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg31/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[12] = {
#include "stg31/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg31/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg31/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg31/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[54] = {
#include "stg31/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg31/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[25] = {
#include "stg31/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg31/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg31/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg31/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[22] = {
#include "stg31/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg31/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[15] = {
#include "stg31/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg31/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg31/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg31/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[22] = {
#include "stg31/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg31/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[15] = {
#include "stg31/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg31/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg31/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg31/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[58] = {
#include "stg31/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg31/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[30] = {
#include "stg31/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg31/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg31/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg31/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[15] = {
#include "stg31/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg31/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[8] = {
#include "stg31/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg31/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg31/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg31/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[18] = {
#include "stg31/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg31/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[9] = {
#include "stg31/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg31/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg31/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg31/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[17] = {
#include "stg31/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg31/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[16] = {
#include "stg31/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg31/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg31/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg31/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[23] = {
#include "stg31/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg31/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[14] = {
#include "stg31/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg31/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg31/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg31/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[23] = {
#include "stg31/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg31/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[14] = {
#include "stg31/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg31/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg31/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg31/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[55] = {
#include "stg31/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg31/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[27] = {
#include "stg31/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg31/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg31/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg31/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[28] = {
#include "stg31/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg31/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[15] = {
#include "stg31/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg31/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg31/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg31/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[98] = {
#include "stg31/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg31/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[43] = {
#include "stg31/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg31/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg31/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg31/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[40] = {
#include "stg31/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg31/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[22] = {
#include "stg31/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg31/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg31/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg31/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[98] = {
#include "stg31/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg31/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[43] = {
#include "stg31/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg31/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg31/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg31/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[32] = {
#include "stg31/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg31/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[16] = {
#include "stg31/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg31/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg31/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg31/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[15] = {
#include "stg31/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg31/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[8] = {
#include "stg31/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg31/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg31/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg31/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[55] = {
#include "stg31/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg31/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[30] = {
#include "stg31/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg31/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg31/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg31/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[23] = {
#include "stg31/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg31/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[15] = {
#include "stg31/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg31/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg31/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg31/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[79] = {
#include "stg31/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg31/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[36] = {
#include "stg31/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg31/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg31/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg31/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[36] = {
#include "stg31/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg31/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[16] = {
#include "stg31/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg31/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg31/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg31/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[107] = {
#include "stg31/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg31/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[44] = {
#include "stg31/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg31/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg31/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg31/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[43] = {
#include "stg31/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg31/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[25] = {
#include "stg31/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg31/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg31/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_114_materials[1] = {
#include "stg31/terrain/collision_114_materials.inc"
};
static Sint16 collision_114_faces[94] = {
#include "stg31/terrain/collision_114_faces.inc"
};
static NJS_MESHSET collision_114_meshsets[1] = {
#include "stg31/terrain/collision_114_meshsets.inc"
};
static NJS_POINT3 collision_114_points[54] = {
#include "stg31/terrain/collision_114_points.inc"
};
static NJS_MODEL collision_114_model = {
#include "stg31/terrain/collision_114_model.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg31/terrain/collision_114.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg31/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[61] = {
#include "stg31/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg31/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[30] = {
#include "stg31/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg31/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg31/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg31/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[54] = {
#include "stg31/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg31/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[29] = {
#include "stg31/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg31/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg31/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg31/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[37] = {
#include "stg31/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg31/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[24] = {
#include "stg31/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg31/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg31/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg31/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[49] = {
#include "stg31/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg31/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[27] = {
#include "stg31/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg31/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg31/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg31/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[137] = {
#include "stg31/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg31/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[68] = {
#include "stg31/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg31/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg31/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg31/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[102] = {
#include "stg31/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg31/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[56] = {
#include "stg31/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg31/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg31/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg31/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[65] = {
#include "stg31/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg31/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[36] = {
#include "stg31/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg31/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg31/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg31/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[71] = {
#include "stg31/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg31/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[34] = {
#include "stg31/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg31/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg31/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg31/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[36] = {
#include "stg31/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg31/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[16] = {
#include "stg31/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg31/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg31/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg31/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[19] = {
#include "stg31/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg31/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[8] = {
#include "stg31/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg31/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg31/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg31/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[36] = {
#include "stg31/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg31/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[22] = {
#include "stg31/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg31/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg31/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg31/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[82] = {
#include "stg31/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg31/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[45] = {
#include "stg31/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg31/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg31/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_145_materials[1] = {
#include "stg31/terrain/collision_145_materials.inc"
};
static Sint16 collision_145_faces[24] = {
#include "stg31/terrain/collision_145_faces.inc"
};
static NJS_MESHSET collision_145_meshsets[1] = {
#include "stg31/terrain/collision_145_meshsets.inc"
};
static NJS_POINT3 collision_145_points[16] = {
#include "stg31/terrain/collision_145_points.inc"
};
static NJS_MODEL collision_145_model = {
#include "stg31/terrain/collision_145_model.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg31/terrain/collision_145.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg31/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[24] = {
#include "stg31/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg31/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[16] = {
#include "stg31/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg31/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg31/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_147_materials[1] = {
#include "stg31/terrain/collision_147_materials.inc"
};
static Sint16 collision_147_faces[40] = {
#include "stg31/terrain/collision_147_faces.inc"
};
static NJS_MESHSET collision_147_meshsets[1] = {
#include "stg31/terrain/collision_147_meshsets.inc"
};
static NJS_POINT3 collision_147_points[26] = {
#include "stg31/terrain/collision_147_points.inc"
};
static NJS_MODEL collision_147_model = {
#include "stg31/terrain/collision_147_model.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg31/terrain/collision_147.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg31/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[101] = {
#include "stg31/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg31/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[57] = {
#include "stg31/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg31/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg31/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg31/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[45] = {
#include "stg31/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg31/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[28] = {
#include "stg31/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg31/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg31/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg31/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[62] = {
#include "stg31/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg31/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[38] = {
#include "stg31/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg31/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg31/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg31/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[82] = {
#include "stg31/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg31/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[38] = {
#include "stg31/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg31/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg31/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg31/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[48] = {
#include "stg31/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg31/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[28] = {
#include "stg31/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg31/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg31/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg31/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[32] = {
#include "stg31/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg31/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[24] = {
#include "stg31/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg31/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg31/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg31/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[120] = {
#include "stg31/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg31/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[68] = {
#include "stg31/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg31/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg31/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg31/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[59] = {
#include "stg31/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg31/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[36] = {
#include "stg31/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg31/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg31/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg31/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[173] = {
#include "stg31/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg31/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[96] = {
#include "stg31/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg31/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg31/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_157_materials[1] = {
#include "stg31/terrain/collision_157_materials.inc"
};
static Sint16 collision_157_faces[109] = {
#include "stg31/terrain/collision_157_faces.inc"
};
static NJS_MESHSET collision_157_meshsets[1] = {
#include "stg31/terrain/collision_157_meshsets.inc"
};
static NJS_POINT3 collision_157_points[60] = {
#include "stg31/terrain/collision_157_points.inc"
};
static NJS_MODEL collision_157_model = {
#include "stg31/terrain/collision_157_model.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg31/terrain/collision_157.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg31/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[44] = {
#include "stg31/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg31/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[34] = {
#include "stg31/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg31/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg31/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg31/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[90] = {
#include "stg31/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg31/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[46] = {
#include "stg31/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg31/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg31/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg31/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[65] = {
#include "stg31/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg31/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[37] = {
#include "stg31/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg31/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg31/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg31/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[87] = {
#include "stg31/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg31/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[44] = {
#include "stg31/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg31/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg31/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg31/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[18] = {
#include "stg31/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg31/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[16] = {
#include "stg31/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg31/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg31/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg31/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[28] = {
#include "stg31/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg31/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[15] = {
#include "stg31/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg31/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg31/terrain/collision_163.inc"
};
static NJS_MATERIAL collision_164_materials[1] = {
#include "stg31/terrain/collision_164_materials.inc"
};
static Sint16 collision_164_faces[127] = {
#include "stg31/terrain/collision_164_faces.inc"
};
static NJS_MESHSET collision_164_meshsets[1] = {
#include "stg31/terrain/collision_164_meshsets.inc"
};
static NJS_POINT3 collision_164_points[69] = {
#include "stg31/terrain/collision_164_points.inc"
};
static NJS_MODEL collision_164_model = {
#include "stg31/terrain/collision_164_model.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg31/terrain/collision_164.inc"
};
static NJS_MATERIAL collision_165_materials[1] = {
#include "stg31/terrain/collision_165_materials.inc"
};
static Sint16 collision_165_faces[92] = {
#include "stg31/terrain/collision_165_faces.inc"
};
static NJS_MESHSET collision_165_meshsets[1] = {
#include "stg31/terrain/collision_165_meshsets.inc"
};
static NJS_POINT3 collision_165_points[49] = {
#include "stg31/terrain/collision_165_points.inc"
};
static NJS_MODEL collision_165_model = {
#include "stg31/terrain/collision_165_model.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg31/terrain/collision_165.inc"
};
static NJS_MATERIAL collision_166_materials[1] = {
#include "stg31/terrain/collision_166_materials.inc"
};
static Sint16 collision_166_faces[63] = {
#include "stg31/terrain/collision_166_faces.inc"
};
static NJS_MESHSET collision_166_meshsets[1] = {
#include "stg31/terrain/collision_166_meshsets.inc"
};
static NJS_POINT3 collision_166_points[44] = {
#include "stg31/terrain/collision_166_points.inc"
};
static NJS_MODEL collision_166_model = {
#include "stg31/terrain/collision_166_model.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg31/terrain/collision_166.inc"
};
static NJS_MATERIAL collision_167_materials[1] = {
#include "stg31/terrain/collision_167_materials.inc"
};
static Sint16 collision_167_faces[44] = {
#include "stg31/terrain/collision_167_faces.inc"
};
static NJS_MESHSET collision_167_meshsets[1] = {
#include "stg31/terrain/collision_167_meshsets.inc"
};
static NJS_POINT3 collision_167_points[25] = {
#include "stg31/terrain/collision_167_points.inc"
};
static NJS_MODEL collision_167_model = {
#include "stg31/terrain/collision_167_model.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg31/terrain/collision_167.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg31/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[33] = {
#include "stg31/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg31/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[20] = {
#include "stg31/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg31/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg31/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg31/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[27] = {
#include "stg31/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg31/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[18] = {
#include "stg31/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg31/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg31/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_170_materials[1] = {
#include "stg31/terrain/collision_170_materials.inc"
};
static Sint16 collision_170_faces[7] = {
#include "stg31/terrain/collision_170_faces.inc"
};
static NJS_MESHSET collision_170_meshsets[1] = {
#include "stg31/terrain/collision_170_meshsets.inc"
};
static NJS_POINT3 collision_170_points[6] = {
#include "stg31/terrain/collision_170_points.inc"
};
static NJS_MODEL collision_170_model = {
#include "stg31/terrain/collision_170_model.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg31/terrain/collision_170.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg31/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[12] = {
#include "stg31/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg31/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[8] = {
#include "stg31/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg31/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg31/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_172_materials[1] = {
#include "stg31/terrain/collision_172_materials.inc"
};
static Sint16 collision_172_faces[13] = {
#include "stg31/terrain/collision_172_faces.inc"
};
static NJS_MESHSET collision_172_meshsets[1] = {
#include "stg31/terrain/collision_172_meshsets.inc"
};
static NJS_POINT3 collision_172_points[9] = {
#include "stg31/terrain/collision_172_points.inc"
};
static NJS_MODEL collision_172_model = {
#include "stg31/terrain/collision_172_model.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg31/terrain/collision_172.inc"
};
static NJS_MATERIAL collision_173_materials[1] = {
#include "stg31/terrain/collision_173_materials.inc"
};
static Sint16 collision_173_faces[9] = {
#include "stg31/terrain/collision_173_faces.inc"
};
static NJS_MESHSET collision_173_meshsets[1] = {
#include "stg31/terrain/collision_173_meshsets.inc"
};
static NJS_POINT3 collision_173_points[8] = {
#include "stg31/terrain/collision_173_points.inc"
};
static NJS_MODEL collision_173_model = {
#include "stg31/terrain/collision_173_model.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg31/terrain/collision_173.inc"
};
static NJS_MATERIAL collision_174_materials[1] = {
#include "stg31/terrain/collision_174_materials.inc"
};
static Sint16 collision_174_faces[50] = {
#include "stg31/terrain/collision_174_faces.inc"
};
static NJS_MESHSET collision_174_meshsets[1] = {
#include "stg31/terrain/collision_174_meshsets.inc"
};
static NJS_POINT3 collision_174_points[18] = {
#include "stg31/terrain/collision_174_points.inc"
};
static NJS_MODEL collision_174_model = {
#include "stg31/terrain/collision_174_model.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg31/terrain/collision_174.inc"
};
static NJS_MATERIAL collision_175_materials[1] = {
#include "stg31/terrain/collision_175_materials.inc"
};
static Sint16 collision_175_faces[284] = {
#include "stg31/terrain/collision_175_faces.inc"
};
static NJS_MESHSET collision_175_meshsets[1] = {
#include "stg31/terrain/collision_175_meshsets.inc"
};
static NJS_POINT3 collision_175_points[108] = {
#include "stg31/terrain/collision_175_points.inc"
};
static NJS_MODEL collision_175_model = {
#include "stg31/terrain/collision_175_model.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg31/terrain/collision_175.inc"
};
static NJS_MATERIAL collision_176_materials[1] = {
#include "stg31/terrain/collision_176_materials.inc"
};
static Sint16 collision_176_faces[26] = {
#include "stg31/terrain/collision_176_faces.inc"
};
static NJS_MESHSET collision_176_meshsets[1] = {
#include "stg31/terrain/collision_176_meshsets.inc"
};
static NJS_POINT3 collision_176_points[24] = {
#include "stg31/terrain/collision_176_points.inc"
};
static NJS_MODEL collision_176_model = {
#include "stg31/terrain/collision_176_model.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg31/terrain/collision_176.inc"
};
static NJS_MATERIAL collision_177_materials[1] = {
#include "stg31/terrain/collision_177_materials.inc"
};
static Sint16 collision_177_faces[7] = {
#include "stg31/terrain/collision_177_faces.inc"
};
static NJS_MESHSET collision_177_meshsets[1] = {
#include "stg31/terrain/collision_177_meshsets.inc"
};
static NJS_POINT3 collision_177_points[6] = {
#include "stg31/terrain/collision_177_points.inc"
};
static NJS_MODEL collision_177_model = {
#include "stg31/terrain/collision_177_model.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg31/terrain/collision_177.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg31/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[62] = {
#include "stg31/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg31/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[25] = {
#include "stg31/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg31/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg31/terrain/collision_178.inc"
};
static Sint16 draw_234_polygons[321] = {
#include "stg31/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[187] = {
#include "stg31/terrain/draw_234_vertices.inc"
};
static NJS_CNK_MODEL draw_234_model = {
#include "stg31/terrain/draw_234_model.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg31/terrain/draw_237.inc"
};
static Sint16 draw_231_polygons[137] = {
#include "stg31/terrain/draw_231_polygons.inc"
};
static Sint32 draw_231_vertices[107] = {
#include "stg31/terrain/draw_231_vertices.inc"
};
static NJS_CNK_MODEL draw_231_model = {
#include "stg31/terrain/draw_231_model.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg31/terrain/draw_233.inc"
};
static Sint16 draw_230_polygons[321] = {
#include "stg31/terrain/draw_230_polygons.inc"
};
static Sint32 draw_230_vertices[187] = {
#include "stg31/terrain/draw_230_vertices.inc"
};
static NJS_CNK_MODEL draw_230_model = {
#include "stg31/terrain/draw_230_model.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg31/terrain/draw_230.inc"
};
static Sint16 draw_229_polygons[137] = {
#include "stg31/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[107] = {
#include "stg31/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg31/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg31/terrain/draw_229.inc"
};
static Sint16 draw_228_polygons[103] = {
#include "stg31/terrain/draw_228_polygons.inc"
};
static Sint32 draw_228_vertices[75] = {
#include "stg31/terrain/draw_228_vertices.inc"
};
static NJS_CNK_MODEL draw_228_model = {
#include "stg31/terrain/draw_228_model.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg31/terrain/draw_228.inc"
};
static Sint16 draw_226_polygons[123] = {
#include "stg31/terrain/draw_226_polygons.inc"
};
static Sint32 draw_226_vertices[83] = {
#include "stg31/terrain/draw_226_vertices.inc"
};
static NJS_CNK_MODEL draw_226_model = {
#include "stg31/terrain/draw_226_model.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg31/terrain/draw_227.inc"
};
static Sint16 draw_225_polygons[123] = {
#include "stg31/terrain/draw_225_polygons.inc"
};
static Sint32 draw_225_vertices[83] = {
#include "stg31/terrain/draw_225_vertices.inc"
};
static NJS_CNK_MODEL draw_225_model = {
#include "stg31/terrain/draw_225_model.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg31/terrain/draw_225.inc"
};
static Sint16 draw_224_polygons[325] = {
#include "stg31/terrain/draw_224_polygons.inc"
};
static Sint32 draw_224_vertices[199] = {
#include "stg31/terrain/draw_224_vertices.inc"
};
static NJS_CNK_MODEL draw_224_model = {
#include "stg31/terrain/draw_224_model.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg31/terrain/draw_224.inc"
};
static Sint16 draw_223_polygons[253] = {
#include "stg31/terrain/draw_223_polygons.inc"
};
static Sint32 draw_223_vertices[143] = {
#include "stg31/terrain/draw_223_vertices.inc"
};
static NJS_CNK_MODEL draw_223_model = {
#include "stg31/terrain/draw_223_model.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg31/terrain/draw_223.inc"
};
static Sint16 draw_222_polygons[141] = {
#include "stg31/terrain/draw_222_polygons.inc"
};
static Sint32 draw_222_vertices[107] = {
#include "stg31/terrain/draw_222_vertices.inc"
};
static NJS_CNK_MODEL draw_222_model = {
#include "stg31/terrain/draw_222_model.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg31/terrain/draw_222.inc"
};
static Sint16 draw_221_polygons[141] = {
#include "stg31/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[107] = {
#include "stg31/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg31/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg31/terrain/draw_221.inc"
};
static Sint16 draw_220_polygons[239] = {
#include "stg31/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[139] = {
#include "stg31/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg31/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg31/terrain/draw_220.inc"
};
static Sint16 draw_219_polygons[191] = {
#include "stg31/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[131] = {
#include "stg31/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg31/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg31/terrain/draw_219.inc"
};
static Sint16 draw_218_polygons[227] = {
#include "stg31/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[135] = {
#include "stg31/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg31/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg31/terrain/draw_218.inc"
};
static Sint16 draw_199_polygons[469] = {
#include "stg31/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[275] = {
#include "stg31/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg31/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg31/terrain/draw_217.inc"
};
static Sint16 draw_198_polygons[213] = {
#include "stg31/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[115] = {
#include "stg31/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg31/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg31/terrain/draw_198.inc"
};
static Sint16 draw_197_polygons[325] = {
#include "stg31/terrain/draw_197_polygons.inc"
};
static Sint32 draw_197_vertices[187] = {
#include "stg31/terrain/draw_197_vertices.inc"
};
static NJS_CNK_MODEL draw_197_model = {
#include "stg31/terrain/draw_197_model.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg31/terrain/draw_197.inc"
};
static Sint16 draw_196_polygons[141] = {
#include "stg31/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[107] = {
#include "stg31/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg31/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg31/terrain/draw_196.inc"
};
static Sint16 draw_195_polygons[345] = {
#include "stg31/terrain/draw_195_polygons.inc"
};
static Sint32 draw_195_vertices[199] = {
#include "stg31/terrain/draw_195_vertices.inc"
};
static NJS_CNK_MODEL draw_195_model = {
#include "stg31/terrain/draw_195_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg31/terrain/draw_195.inc"
};
static Sint16 draw_193_polygons[325] = {
#include "stg31/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[199] = {
#include "stg31/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg31/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg31/terrain/draw_194.inc"
};
static Sint16 draw_192_polygons[141] = {
#include "stg31/terrain/draw_192_polygons.inc"
};
static Sint32 draw_192_vertices[107] = {
#include "stg31/terrain/draw_192_vertices.inc"
};
static NJS_CNK_MODEL draw_192_model = {
#include "stg31/terrain/draw_192_model.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg31/terrain/draw_192.inc"
};
static Sint16 draw_191_polygons[157] = {
#include "stg31/terrain/draw_191_polygons.inc"
};
static Sint32 draw_191_vertices[111] = {
#include "stg31/terrain/draw_191_vertices.inc"
};
static NJS_CNK_MODEL draw_191_model = {
#include "stg31/terrain/draw_191_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg31/terrain/draw_191.inc"
};
static Sint16 draw_190_polygons[225] = {
#include "stg31/terrain/draw_190_polygons.inc"
};
static Sint32 draw_190_vertices[167] = {
#include "stg31/terrain/draw_190_vertices.inc"
};
static NJS_CNK_MODEL draw_190_model = {
#include "stg31/terrain/draw_190_model.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg31/terrain/draw_190.inc"
};
static Sint16 draw_189_polygons[169] = {
#include "stg31/terrain/draw_189_polygons.inc"
};
static Sint32 draw_189_vertices[107] = {
#include "stg31/terrain/draw_189_vertices.inc"
};
static NJS_CNK_MODEL draw_189_model = {
#include "stg31/terrain/draw_189_model.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg31/terrain/draw_189.inc"
};
static Sint16 draw_183_polygons[333] = {
#include "stg31/terrain/draw_183_polygons.inc"
};
static Sint32 draw_183_vertices[195] = {
#include "stg31/terrain/draw_183_vertices.inc"
};
static NJS_CNK_MODEL draw_183_model = {
#include "stg31/terrain/draw_183_model.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg31/terrain/draw_188.inc"
};
static Sint16 draw_181_polygons[427] = {
#include "stg31/terrain/draw_181_polygons.inc"
};
static Sint32 draw_181_vertices[235] = {
#include "stg31/terrain/draw_181_vertices.inc"
};
static NJS_CNK_MODEL draw_181_model = {
#include "stg31/terrain/draw_181_model.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg31/terrain/draw_182.inc"
};
static Sint16 draw_179_polygons[121] = {
#include "stg31/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[131] = {
#include "stg31/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg31/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg31/terrain/draw_180.inc"
};
static Sint16 draw_178_polygons[85] = {
#include "stg31/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[47] = {
#include "stg31/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg31/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg31/terrain/draw_178.inc"
};
static Sint16 draw_176_polygons[135] = {
#include "stg31/terrain/draw_176_polygons.inc"
};
static Sint32 draw_176_vertices[99] = {
#include "stg31/terrain/draw_176_vertices.inc"
};
static NJS_CNK_MODEL draw_176_model = {
#include "stg31/terrain/draw_176_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg31/terrain/draw_177.inc"
};
static Sint16 draw_175_polygons[81] = {
#include "stg31/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[71] = {
#include "stg31/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg31/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg31/terrain/draw_175.inc"
};
static Sint16 draw_174_polygons[505] = {
#include "stg31/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[391] = {
#include "stg31/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg31/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg31/terrain/draw_174.inc"
};
static Sint16 draw_172_polygons[167] = {
#include "stg31/terrain/draw_172_polygons.inc"
};
static Sint32 draw_172_vertices[119] = {
#include "stg31/terrain/draw_172_vertices.inc"
};
static NJS_CNK_MODEL draw_172_model = {
#include "stg31/terrain/draw_172_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg31/terrain/draw_173.inc"
};
static Sint16 draw_166_polygons[537] = {
#include "stg31/terrain/draw_166_polygons.inc"
};
static Sint32 draw_166_vertices[367] = {
#include "stg31/terrain/draw_166_vertices.inc"
};
static NJS_CNK_MODEL draw_166_model = {
#include "stg31/terrain/draw_166_model.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg31/terrain/draw_171.inc"
};
static Sint16 draw_165_polygons[131] = {
#include "stg31/terrain/draw_165_polygons.inc"
};
static Sint32 draw_165_vertices[123] = {
#include "stg31/terrain/draw_165_vertices.inc"
};
static NJS_CNK_MODEL draw_165_model = {
#include "stg31/terrain/draw_165_model.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg31/terrain/draw_165.inc"
};
static Sint16 draw_164_polygons[93] = {
#include "stg31/terrain/draw_164_polygons.inc"
};
static Sint32 draw_164_vertices[67] = {
#include "stg31/terrain/draw_164_vertices.inc"
};
static NJS_CNK_MODEL draw_164_model = {
#include "stg31/terrain/draw_164_model.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg31/terrain/draw_164.inc"
};
static Sint16 draw_161_polygons[141] = {
#include "stg31/terrain/draw_161_polygons.inc"
};
static Sint32 draw_161_vertices[107] = {
#include "stg31/terrain/draw_161_vertices.inc"
};
static NJS_CNK_MODEL draw_161_model = {
#include "stg31/terrain/draw_161_model.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg31/terrain/draw_163.inc"
};
static Sint16 draw_160_polygons[247] = {
#include "stg31/terrain/draw_160_polygons.inc"
};
static Sint32 draw_160_vertices[163] = {
#include "stg31/terrain/draw_160_vertices.inc"
};
static NJS_CNK_MODEL draw_160_model = {
#include "stg31/terrain/draw_160_model.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg31/terrain/draw_160.inc"
};
static Sint16 draw_159_polygons[141] = {
#include "stg31/terrain/draw_159_polygons.inc"
};
static Sint32 draw_159_vertices[107] = {
#include "stg31/terrain/draw_159_vertices.inc"
};
static NJS_CNK_MODEL draw_159_model = {
#include "stg31/terrain/draw_159_model.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg31/terrain/draw_159.inc"
};
static Sint16 draw_158_polygons[105] = {
#include "stg31/terrain/draw_158_polygons.inc"
};
static Sint32 draw_158_vertices[67] = {
#include "stg31/terrain/draw_158_vertices.inc"
};
static NJS_CNK_MODEL draw_158_model = {
#include "stg31/terrain/draw_158_model.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg31/terrain/draw_158.inc"
};
static Sint16 draw_157_polygons[469] = {
#include "stg31/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[275] = {
#include "stg31/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg31/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg31/terrain/draw_157.inc"
};
static Sint16 draw_156_polygons[349] = {
#include "stg31/terrain/draw_156_polygons.inc"
};
static Sint32 draw_156_vertices[235] = {
#include "stg31/terrain/draw_156_vertices.inc"
};
static NJS_CNK_MODEL draw_156_model = {
#include "stg31/terrain/draw_156_model.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg31/terrain/draw_156.inc"
};
static Sint16 draw_155_polygons[441] = {
#include "stg31/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[243] = {
#include "stg31/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg31/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg31/terrain/draw_155.inc"
};
static Sint16 draw_154_polygons[471] = {
#include "stg31/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[275] = {
#include "stg31/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg31/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg31/terrain/draw_154.inc"
};
static Sint16 draw_153_polygons[443] = {
#include "stg31/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[235] = {
#include "stg31/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg31/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg31/terrain/draw_153.inc"
};
static Sint16 draw_152_polygons[105] = {
#include "stg31/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[67] = {
#include "stg31/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg31/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg31/terrain/draw_152.inc"
};
static Sint16 draw_151_polygons[105] = {
#include "stg31/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[67] = {
#include "stg31/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg31/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg31/terrain/draw_151.inc"
};
static Sint16 draw_150_polygons[325] = {
#include "stg31/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[191] = {
#include "stg31/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg31/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg31/terrain/draw_150.inc"
};
static Sint16 draw_148_polygons[121] = {
#include "stg31/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[127] = {
#include "stg31/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg31/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg31/terrain/draw_149.inc"
};
static Sint16 draw_147_polygons[195] = {
#include "stg31/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[119] = {
#include "stg31/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg31/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg31/terrain/draw_147.inc"
};
static Sint16 draw_146_polygons[2169] = {
#include "stg31/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[1779] = {
#include "stg31/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg31/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg31/terrain/draw_146.inc"
};
static Sint16 draw_145_polygons[63] = {
#include "stg31/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[47] = {
#include "stg31/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg31/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg31/terrain/draw_145.inc"
};
static Sint16 draw_144_polygons[29] = {
#include "stg31/terrain/draw_144_polygons.inc"
};
static Sint32 draw_144_vertices[27] = {
#include "stg31/terrain/draw_144_vertices.inc"
};
static NJS_CNK_MODEL draw_144_model = {
#include "stg31/terrain/draw_144_model.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg31/terrain/draw_144.inc"
};
static Sint16 draw_143_polygons[167] = {
#include "stg31/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[119] = {
#include "stg31/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg31/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg31/terrain/draw_143.inc"
};
static Sint16 draw_140_polygons[681] = {
#include "stg31/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[471] = {
#include "stg31/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg31/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg31/terrain/draw_142.inc"
};
static Sint16 draw_138_polygons[463] = {
#include "stg31/terrain/draw_138_polygons.inc"
};
static Sint32 draw_138_vertices[299] = {
#include "stg31/terrain/draw_138_vertices.inc"
};
static NJS_CNK_MODEL draw_138_model = {
#include "stg31/terrain/draw_138_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg31/terrain/draw_139.inc"
};
static Sint16 draw_137_polygons[553] = {
#include "stg31/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[527] = {
#include "stg31/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg31/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg31/terrain/draw_137.inc"
};
static Sint16 draw_136_polygons[233] = {
#include "stg31/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[207] = {
#include "stg31/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg31/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg31/terrain/draw_136.inc"
};
static Sint16 draw_134_polygons[217] = {
#include "stg31/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[199] = {
#include "stg31/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg31/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg31/terrain/draw_135.inc"
};
static Sint16 draw_131_polygons[393] = {
#include "stg31/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[387] = {
#include "stg31/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg31/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg31/terrain/draw_133.inc"
};
static Sint16 draw_130_polygons[83] = {
#include "stg31/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[75] = {
#include "stg31/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg31/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg31/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[171] = {
#include "stg31/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[163] = {
#include "stg31/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg31/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg31/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[137] = {
#include "stg31/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[107] = {
#include "stg31/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg31/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg31/terrain/draw_128.inc"
};
static Sint16 draw_126_polygons[393] = {
#include "stg31/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[387] = {
#include "stg31/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg31/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg31/terrain/draw_127.inc"
};
static Sint16 draw_125_polygons[97] = {
#include "stg31/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[95] = {
#include "stg31/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg31/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg31/terrain/draw_125.inc"
};
static Sint16 draw_124_polygons[317] = {
#include "stg31/terrain/draw_124_polygons.inc"
};
static Sint32 draw_124_vertices[187] = {
#include "stg31/terrain/draw_124_vertices.inc"
};
static NJS_CNK_MODEL draw_124_model = {
#include "stg31/terrain/draw_124_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg31/terrain/draw_124.inc"
};
static Sint16 draw_123_polygons[317] = {
#include "stg31/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[187] = {
#include "stg31/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg31/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg31/terrain/draw_123.inc"
};
static Sint16 draw_117_polygons[105] = {
#include "stg31/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[67] = {
#include "stg31/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg31/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg31/terrain/draw_122.inc"
};
static Sint16 draw_116_polygons[329] = {
#include "stg31/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[323] = {
#include "stg31/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg31/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg31/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[205] = {
#include "stg31/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[159] = {
#include "stg31/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg31/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg31/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[413] = {
#include "stg31/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[391] = {
#include "stg31/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg31/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg31/terrain/draw_114.inc"
};
static Sint16 draw_112_polygons[233] = {
#include "stg31/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[131] = {
#include "stg31/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg31/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg31/terrain/draw_113.inc"
};
static Sint16 draw_110_polygons[217] = {
#include "stg31/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[219] = {
#include "stg31/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg31/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg31/terrain/draw_111.inc"
};
static Sint16 draw_109_polygons[217] = {
#include "stg31/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[199] = {
#include "stg31/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg31/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg31/terrain/draw_109.inc"
};
static Sint16 draw_108_polygons[463] = {
#include "stg31/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[267] = {
#include "stg31/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg31/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg31/terrain/draw_108.inc"
};
static Sint16 draw_106_polygons[413] = {
#include "stg31/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[375] = {
#include "stg31/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg31/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg31/terrain/draw_107.inc"
};
static Sint16 draw_105_polygons[727] = {
#include "stg31/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[415] = {
#include "stg31/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg31/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg31/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[439] = {
#include "stg31/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[475] = {
#include "stg31/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg31/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg31/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[265] = {
#include "stg31/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[191] = {
#include "stg31/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg31/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg31/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[121] = {
#include "stg31/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[131] = {
#include "stg31/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg31/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg31/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[193] = {
#include "stg31/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[171] = {
#include "stg31/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg31/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg31/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[487] = {
#include "stg31/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[399] = {
#include "stg31/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg31/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg31/terrain/draw_100.inc"
};
static Sint16 draw_98_polygons[109] = {
#include "stg31/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[87] = {
#include "stg31/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg31/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg31/terrain/draw_99.inc"
};
static Sint16 draw_97_polygons[35] = {
#include "stg31/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[35] = {
#include "stg31/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg31/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg31/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[197] = {
#include "stg31/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[123] = {
#include "stg31/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg31/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg31/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[321] = {
#include "stg31/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[311] = {
#include "stg31/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg31/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg31/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[175] = {
#include "stg31/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[111] = {
#include "stg31/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg31/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg31/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[439] = {
#include "stg31/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[363] = {
#include "stg31/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg31/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg31/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[421] = {
#include "stg31/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[267] = {
#include "stg31/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg31/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg31/terrain/draw_92.inc"
};
static Sint16 draw_89_polygons[2169] = {
#include "stg31/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[1795] = {
#include "stg31/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg31/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg31/terrain/draw_91.inc"
};
static Sint16 draw_88_polygons[101] = {
#include "stg31/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[67] = {
#include "stg31/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg31/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg31/terrain/draw_88.inc"
};
static Sint16 draw_87_polygons[113] = {
#include "stg31/terrain/draw_87_polygons.inc"
};
static Sint32 draw_87_vertices[95] = {
#include "stg31/terrain/draw_87_vertices.inc"
};
static NJS_CNK_MODEL draw_87_model = {
#include "stg31/terrain/draw_87_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg31/terrain/draw_87.inc"
};
static Sint16 draw_86_polygons[95] = {
#include "stg31/terrain/draw_86_polygons.inc"
};
static Sint32 draw_86_vertices[75] = {
#include "stg31/terrain/draw_86_vertices.inc"
};
static NJS_CNK_MODEL draw_86_model = {
#include "stg31/terrain/draw_86_model.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg31/terrain/draw_86.inc"
};
static Sint16 draw_85_polygons[135] = {
#include "stg31/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[127] = {
#include "stg31/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg31/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg31/terrain/draw_85.inc"
};
static Sint16 draw_84_polygons[105] = {
#include "stg31/terrain/draw_84_polygons.inc"
};
static Sint32 draw_84_vertices[95] = {
#include "stg31/terrain/draw_84_vertices.inc"
};
static NJS_CNK_MODEL draw_84_model = {
#include "stg31/terrain/draw_84_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg31/terrain/draw_84.inc"
};
static Sint16 draw_83_polygons[285] = {
#include "stg31/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[259] = {
#include "stg31/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg31/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg31/terrain/draw_83.inc"
};
static Sint16 draw_82_polygons[177] = {
#include "stg31/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[143] = {
#include "stg31/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg31/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg31/terrain/draw_82.inc"
};
static Sint16 draw_81_polygons[123] = {
#include "stg31/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[115] = {
#include "stg31/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg31/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg31/terrain/draw_81.inc"
};
static Sint16 draw_80_polygons[119] = {
#include "stg31/terrain/draw_80_polygons.inc"
};
static Sint32 draw_80_vertices[87] = {
#include "stg31/terrain/draw_80_vertices.inc"
};
static NJS_CNK_MODEL draw_80_model = {
#include "stg31/terrain/draw_80_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg31/terrain/draw_80.inc"
};
static Sint16 draw_79_polygons[155] = {
#include "stg31/terrain/draw_79_polygons.inc"
};
static Sint32 draw_79_vertices[159] = {
#include "stg31/terrain/draw_79_vertices.inc"
};
static NJS_CNK_MODEL draw_79_model = {
#include "stg31/terrain/draw_79_model.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg31/terrain/draw_79.inc"
};
static Sint16 draw_78_polygons[151] = {
#include "stg31/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[139] = {
#include "stg31/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg31/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg31/terrain/draw_78.inc"
};
static Sint16 draw_76_polygons[49] = {
#include "stg31/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[51] = {
#include "stg31/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg31/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg31/terrain/draw_77.inc"
};
static Sint16 draw_75_polygons[49] = {
#include "stg31/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[51] = {
#include "stg31/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg31/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg31/terrain/draw_75.inc"
};
static Sint16 draw_74_polygons[95] = {
#include "stg31/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[75] = {
#include "stg31/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg31/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg31/terrain/draw_74.inc"
};
static Sint16 draw_73_polygons[289] = {
#include "stg31/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[227] = {
#include "stg31/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg31/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg31/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[179] = {
#include "stg31/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[191] = {
#include "stg31/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg31/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg31/terrain/draw_72.inc"
};
static Sint16 draw_71_polygons[285] = {
#include "stg31/terrain/draw_71_polygons.inc"
};
static Sint32 draw_71_vertices[199] = {
#include "stg31/terrain/draw_71_vertices.inc"
};
static NJS_CNK_MODEL draw_71_model = {
#include "stg31/terrain/draw_71_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg31/terrain/draw_71.inc"
};
static Sint16 draw_70_polygons[215] = {
#include "stg31/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[247] = {
#include "stg31/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg31/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg31/terrain/draw_70.inc"
};
static Sint16 draw_69_polygons[63] = {
#include "stg31/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[51] = {
#include "stg31/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg31/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg31/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[119] = {
#include "stg31/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[99] = {
#include "stg31/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg31/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg31/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[103] = {
#include "stg31/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[99] = {
#include "stg31/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg31/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg31/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[139] = {
#include "stg31/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[139] = {
#include "stg31/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg31/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg31/terrain/draw_66.inc"
};
static Sint16 draw_64_polygons[157] = {
#include "stg31/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[163] = {
#include "stg31/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg31/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg31/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[125] = {
#include "stg31/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[99] = {
#include "stg31/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg31/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg31/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[135] = {
#include "stg31/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[111] = {
#include "stg31/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg31/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg31/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[49] = {
#include "stg31/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[51] = {
#include "stg31/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg31/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg31/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[77] = {
#include "stg31/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[75] = {
#include "stg31/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg31/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg31/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[121] = {
#include "stg31/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[111] = {
#include "stg31/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg31/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg31/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[81] = {
#include "stg31/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[75] = {
#include "stg31/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg31/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg31/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[277] = {
#include "stg31/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[235] = {
#include "stg31/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg31/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg31/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[137] = {
#include "stg31/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[99] = {
#include "stg31/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg31/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg31/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[107] = {
#include "stg31/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[91] = {
#include "stg31/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg31/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg31/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[87] = {
#include "stg31/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[75] = {
#include "stg31/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg31/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg31/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[75] = {
#include "stg31/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[63] = {
#include "stg31/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg31/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg31/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[125] = {
#include "stg31/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[103] = {
#include "stg31/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg31/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg31/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[43] = {
#include "stg31/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[35] = {
#include "stg31/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg31/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg31/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[253] = {
#include "stg31/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[143] = {
#include "stg31/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg31/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg31/terrain/draw_50.inc"
};
static Sint16 draw_39_polygons[119] = {
#include "stg31/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[123] = {
#include "stg31/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg31/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg31/terrain/draw_49.inc"
};
static Sint16 draw_31_polygons[127] = {
#include "stg31/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[127] = {
#include "stg31/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg31/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg31/terrain/draw_38.inc"
};
static Sint16 draw_30_polygons[1011] = {
#include "stg31/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[875] = {
#include "stg31/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg31/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg31/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[207] = {
#include "stg31/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[207] = {
#include "stg31/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg31/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg31/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[979] = {
#include "stg31/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[607] = {
#include "stg31/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg31/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg31/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[139] = {
#include "stg31/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[131] = {
#include "stg31/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg31/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg31/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[321] = {
#include "stg31/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[187] = {
#include "stg31/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg31/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg31/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[359] = {
#include "stg31/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[347] = {
#include "stg31/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg31/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg31/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[541] = {
#include "stg31/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[511] = {
#include "stg31/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg31/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg31/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[471] = {
#include "stg31/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[471] = {
#include "stg31/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg31/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg31/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[91] = {
#include "stg31/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[87] = {
#include "stg31/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg31/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg31/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[321] = {
#include "stg31/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[323] = {
#include "stg31/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg31/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg31/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[661] = {
#include "stg31/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[591] = {
#include "stg31/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg31/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg31/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[517] = {
#include "stg31/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[323] = {
#include "stg31/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg31/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg31/terrain/draw_19.inc"
};
static Sint16 draw_18_polygons[1555] = {
#include "stg31/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[1443] = {
#include "stg31/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg31/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg31/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[1431] = {
#include "stg31/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[1423] = {
#include "stg31/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg31/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg31/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[505] = {
#include "stg31/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[331] = {
#include "stg31/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg31/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg31/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[213] = {
#include "stg31/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[115] = {
#include "stg31/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg31/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg31/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[1231] = {
#include "stg31/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[847] = {
#include "stg31/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg31/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg31/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[1215] = {
#include "stg31/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[1199] = {
#include "stg31/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg31/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg31/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[635] = {
#include "stg31/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[383] = {
#include "stg31/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg31/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg31/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[505] = {
#include "stg31/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[411] = {
#include "stg31/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg31/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg31/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[535] = {
#include "stg31/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[355] = {
#include "stg31/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg31/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg31/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[151] = {
#include "stg31/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[139] = {
#include "stg31/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg31/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg31/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[119] = {
#include "stg31/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[99] = {
#include "stg31/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg31/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg31/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[49] = {
#include "stg31/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[39] = {
#include "stg31/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg31/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg31/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[105] = {
#include "stg31/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[99] = {
#include "stg31/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg31/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg31/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[103] = {
#include "stg31/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[75] = {
#include "stg31/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg31/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg31/terrain/draw_5.inc"
};
static Sint16 draw_2_polygons[293] = {
#include "stg31/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[227] = {
#include "stg31/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg31/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg31/terrain/draw_4.inc"
};
static Sint16 draw_0_polygons[491] = {
#include "stg31/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[255] = {
#include "stg31/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg31/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg31/terrain/draw_1.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg31/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg31/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg31/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg31/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg31/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg31/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg31/terrain/collision_15.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg31/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg31/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg31/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg31/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg31/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg31/terrain/collision_22.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg31/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg31/terrain/collision_27.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg31/terrain/collision_31.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg31/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg31/terrain/collision_36.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg31/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg31/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg31/terrain/collision_46.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg31/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg31/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg31/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg31/terrain/collision_55.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg31/terrain/collision_57.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg31/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg31/terrain/collision_61.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg31/terrain/collision_64.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg31/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg31/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg31/terrain/collision_71.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg31/terrain/collision_76.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg31/terrain/collision_78.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg31/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg31/terrain/collision_83.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg31/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg31/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg31/terrain/collision_87.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg31/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg31/terrain/collision_90.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg31/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg31/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg31/terrain/collision_96.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg31/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg31/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg31/terrain/collision_100.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg31/terrain/collision_101.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg31/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg31/terrain/collision_103.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg31/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg31/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg31/terrain/collision_106.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg31/terrain/collision_109.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg31/terrain/collision_115.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg31/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg31/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg31/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg31/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg31/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg31/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg31/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg31/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg31/terrain/collision_134.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg31/terrain/collision_135.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg31/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg31/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg31/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg31/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg31/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg31/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg31/terrain/collision_142.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg31/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg31/terrain/collision_180.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg31/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg31/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg31/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg31/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg31/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg31/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg31/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg31/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg31/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg31/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg31/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg31/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg31/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg31/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg31/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg31/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg31/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg31/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg31/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg31/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg31/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg31/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg31/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg31/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg31/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg31/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg31/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg31/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg31/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg31/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg31/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg31/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg31/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg31/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg31/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg31/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg31/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg31/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg31/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg31/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg31/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg31/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg31/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg31/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg31/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg31/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg31/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg31/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg31/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg31/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg31/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg31/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg31/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg31/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg31/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg31/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg31/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg31/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg31/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg31/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg31/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg31/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg31/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg31/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg31/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg31/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg31/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg31/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg31/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg31/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg31/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg31/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg31/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg31/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg31/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg31/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg31/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg31/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg31/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg31/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg31/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg31/terrain/draw_0.inc"
};
