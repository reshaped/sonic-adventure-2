#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg32/meteor_herd.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char meteor_herd_land_filenames[706] = {
#include "stg32/terrain/meteor_herd_land_filenames.inc"
};

/* STG32.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between meteor_herd.c and sky_changes.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[347];
struct land_table meteor_herd_land;
static NJS_TEXNAME textures[55];
NJS_TEXLIST meteor_herd_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[197];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[86];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[64];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[33];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[50];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[24];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[171];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[78];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[79];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[38];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[170];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[77];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[27];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[16];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[166];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[75];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[19];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[16];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[74];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[34];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[40];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[18];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[19];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[11];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[252];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[108];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[7];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[6];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[60];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[36];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[42];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[16];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[60];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[36];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[14];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[10];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[23];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[20];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[25];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[24];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[14];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[12];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[49];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[24];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[78];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[40];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[14];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[8];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[15];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[14];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[40];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[27];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[140];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[88];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[7];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[6];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[21];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[12];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[52];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[21];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[37];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[24];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[44];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[32];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_66_materials[1];
static Sint16 collision_66_faces[14];
static NJS_MESHSET collision_66_meshsets[1];
static NJS_POINT3 collision_66_points[12];
static NJS_MODEL collision_66_model;
static NJS_OBJECT collision_66;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[58];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[26];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_68_materials[1];
static Sint16 collision_68_faces[14];
static NJS_MESHSET collision_68_meshsets[1];
static NJS_POINT3 collision_68_points[8];
static NJS_MODEL collision_68_model;
static NJS_OBJECT collision_68;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[24];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[22];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[25];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[20];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[140];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[88];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[7];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[6];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[21];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[12];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[42];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[24];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[47];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[34];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[14];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[12];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[33];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[20];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[7];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[6];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_MATERIAL collision_85_materials[1];
static Sint16 collision_85_faces[18];
static NJS_MESHSET collision_85_meshsets[1];
static NJS_POINT3 collision_85_points[12];
static NJS_MODEL collision_85_model;
static NJS_OBJECT collision_85;
static NJS_MATERIAL collision_86_materials[1];
static Sint16 collision_86_faces[14];
static NJS_MESHSET collision_86_meshsets[1];
static NJS_POINT3 collision_86_points[12];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[7];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[6];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[14];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[9];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_89_materials[1];
static Sint16 collision_89_faces[18];
static NJS_MESHSET collision_89_meshsets[1];
static NJS_POINT3 collision_89_points[12];
static NJS_MODEL collision_89_model;
static NJS_OBJECT collision_89;
static NJS_MATERIAL collision_90_materials[1];
static Sint16 collision_90_faces[13];
static NJS_MESHSET collision_90_meshsets[1];
static NJS_POINT3 collision_90_points[8];
static NJS_MODEL collision_90_model;
static NJS_OBJECT collision_90;
static NJS_MATERIAL collision_91_materials[1];
static Sint16 collision_91_faces[143];
static NJS_MESHSET collision_91_meshsets[1];
static NJS_POINT3 collision_91_points[72];
static NJS_MODEL collision_91_model;
static NJS_OBJECT collision_91;
static NJS_MATERIAL collision_92_materials[1];
static Sint16 collision_92_faces[167];
static NJS_MESHSET collision_92_meshsets[1];
static NJS_POINT3 collision_92_points[64];
static NJS_MODEL collision_92_model;
static NJS_OBJECT collision_92;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[90];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[40];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_101_materials[1];
static Sint16 collision_101_faces[21];
static NJS_MESHSET collision_101_meshsets[1];
static NJS_POINT3 collision_101_points[11];
static NJS_MODEL collision_101_model;
static NJS_OBJECT collision_101;
static NJS_MATERIAL collision_104_materials[1];
static Sint16 collision_104_faces[60];
static NJS_MESHSET collision_104_meshsets[1];
static NJS_POINT3 collision_104_points[36];
static NJS_MODEL collision_104_model;
static NJS_OBJECT collision_104;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[134];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[49];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[7];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[6];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[30];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[21];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[35];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[34];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[70];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[51];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[35];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[34];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[35];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[34];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_126_materials[1];
static Sint16 collision_126_faces[35];
static NJS_MESHSET collision_126_meshsets[1];
static NJS_POINT3 collision_126_points[34];
static NJS_MODEL collision_126_model;
static NJS_OBJECT collision_126;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[14];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[9];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[60];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[36];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[17];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[15];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[12];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[8];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_138_materials[1];
static Sint16 collision_138_faces[39];
static NJS_MESHSET collision_138_meshsets[1];
static NJS_POINT3 collision_138_points[28];
static NJS_MODEL collision_138_model;
static NJS_OBJECT collision_138;
static NJS_MATERIAL collision_139_materials[1];
static Sint16 collision_139_faces[59];
static NJS_MESHSET collision_139_meshsets[1];
static NJS_POINT3 collision_139_points[33];
static NJS_MODEL collision_139_model;
static NJS_OBJECT collision_139;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[11];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[10];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_141_materials[1];
static Sint16 collision_141_faces[51];
static NJS_MESHSET collision_141_meshsets[1];
static NJS_POINT3 collision_141_points[31];
static NJS_MODEL collision_141_model;
static NJS_OBJECT collision_141;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[12];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[8];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[18];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[16];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[11];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[10];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_145_materials[1];
static Sint16 collision_145_faces[23];
static NJS_MESHSET collision_145_meshsets[1];
static NJS_POINT3 collision_145_points[15];
static NJS_MODEL collision_145_model;
static NJS_OBJECT collision_145;
static Sint16 draw_196_polygons[43];
static Sint32 draw_196_vertices[35];
static NJS_CNK_MODEL draw_196_model;
static NJS_CNK_OBJECT draw_196;
static Sint16 draw_194_polygons[257];
static Sint32 draw_194_vertices[187];
static NJS_CNK_MODEL draw_194_model;
static NJS_CNK_OBJECT draw_195;
static Sint16 draw_192_polygons[125];
static Sint32 draw_192_vertices[99];
static NJS_CNK_MODEL draw_192_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_188_polygons[55];
static Sint32 draw_188_vertices[51];
static NJS_CNK_MODEL draw_188_model;
static NJS_CNK_OBJECT draw_191;
static Sint16 draw_187_polygons[71];
static Sint32 draw_187_vertices[75];
static NJS_CNK_MODEL draw_187_model;
static NJS_CNK_OBJECT draw_187;
static Sint16 draw_186_polygons[347];
static Sint32 draw_186_vertices[195];
static NJS_CNK_MODEL draw_186_model;
static NJS_CNK_OBJECT draw_186;
static Sint16 draw_185_polygons[157];
static Sint32 draw_185_vertices[107];
static NJS_CNK_MODEL draw_185_model;
static NJS_CNK_OBJECT draw_185;
static Sint16 draw_184_polygons[57];
static Sint32 draw_184_vertices[55];
static NJS_CNK_MODEL draw_184_model;
static NJS_CNK_OBJECT draw_184;
static Sint16 draw_181_polygons[225];
static Sint32 draw_181_vertices[163];
static NJS_CNK_MODEL draw_181_model;
static NJS_CNK_OBJECT draw_183;
static Sint16 draw_180_polygons[205];
static Sint32 draw_180_vertices[195];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_179_polygons[129];
static Sint32 draw_179_vertices[139];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_175_polygons[473];
static Sint32 draw_175_vertices[323];
static NJS_CNK_MODEL draw_175_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_171_polygons[55];
static Sint32 draw_171_vertices[43];
static NJS_CNK_MODEL draw_171_model;
static NJS_CNK_OBJECT draw_174;
static Sint16 draw_170_polygons[47];
static Sint32 draw_170_vertices[51];
static NJS_CNK_MODEL draw_170_model;
static NJS_CNK_OBJECT draw_170;
static Sint16 draw_169_polygons[213];
static Sint32 draw_169_vertices[147];
static NJS_CNK_MODEL draw_169_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_168_polygons[211];
static Sint32 draw_168_vertices[127];
static NJS_CNK_MODEL draw_168_model;
static NJS_CNK_OBJECT draw_168;
static Sint16 draw_167_polygons[37];
static Sint32 draw_167_vertices[35];
static NJS_CNK_MODEL draw_167_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_155_polygons[157];
static Sint32 draw_155_vertices[139];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_166;
static Sint16 draw_154_polygons[67];
static Sint32 draw_154_vertices[63];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_153_polygons[35];
static Sint32 draw_153_vertices[35];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_151_polygons[167];
static Sint32 draw_151_vertices[103];
static NJS_CNK_MODEL draw_151_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_150_polygons[55];
static Sint32 draw_150_vertices[51];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_150;
static Sint16 draw_149_polygons[71];
static Sint32 draw_149_vertices[75];
static NJS_CNK_MODEL draw_149_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_148_polygons[89];
static Sint32 draw_148_vertices[67];
static NJS_CNK_MODEL draw_148_model;
static NJS_CNK_OBJECT draw_148;
static Sint16 draw_146_polygons[115];
static Sint32 draw_146_vertices[103];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_147;
static Sint16 draw_143_polygons[111];
static Sint32 draw_143_vertices[83];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_141_polygons[357];
static Sint32 draw_141_vertices[247];
static NJS_CNK_MODEL draw_141_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_140_polygons[163];
static Sint32 draw_140_vertices[195];
static NJS_CNK_MODEL draw_140_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_139_polygons[579];
static Sint32 draw_139_vertices[411];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_139;
static Sint16 draw_135_polygons[103];
static Sint32 draw_135_vertices[83];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_134_polygons[127];
static Sint32 draw_134_vertices[99];
static NJS_CNK_MODEL draw_134_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_133_polygons[261];
static Sint32 draw_133_vertices[147];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_133;
static Sint16 draw_132_polygons[61];
static Sint32 draw_132_vertices[67];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[49];
static Sint32 draw_131_vertices[51];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_119_polygons[101];
static Sint32 draw_119_vertices[95];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_118_polygons[67];
static Sint32 draw_118_vertices[51];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_117_polygons[61];
static Sint32 draw_117_vertices[67];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_116_polygons[55];
static Sint32 draw_116_vertices[51];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[71];
static Sint32 draw_115_vertices[75];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_114_polygons[75];
static Sint32 draw_114_vertices[51];
static NJS_CNK_MODEL draw_114_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_112_polygons[165];
static Sint32 draw_112_vertices[159];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_113;
static Sint16 draw_108_polygons[187];
static Sint32 draw_108_vertices[139];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_107_polygons[185];
static Sint32 draw_107_vertices[195];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_106_polygons[125];
static Sint32 draw_106_vertices[99];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[61];
static Sint32 draw_105_vertices[67];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[37];
static Sint32 draw_104_vertices[35];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[55];
static Sint32 draw_103_vertices[51];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[41];
static Sint32 draw_102_vertices[35];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_101_polygons[103];
static Sint32 draw_101_vertices[75];
static NJS_CNK_MODEL draw_101_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_100_polygons[187];
static Sint32 draw_100_vertices[203];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_100;
static Sint16 draw_99_polygons[211];
static Sint32 draw_99_vertices[131];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_98_polygons[51];
static Sint32 draw_98_vertices[51];
static NJS_CNK_MODEL draw_98_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_97_polygons[55];
static Sint32 draw_97_vertices[51];
static NJS_CNK_MODEL draw_97_model;
static NJS_CNK_OBJECT draw_97;
static Sint16 draw_96_polygons[47];
static Sint32 draw_96_vertices[43];
static NJS_CNK_MODEL draw_96_model;
static NJS_CNK_OBJECT draw_96;
static Sint16 draw_95_polygons[171];
static Sint32 draw_95_vertices[195];
static NJS_CNK_MODEL draw_95_model;
static NJS_CNK_OBJECT draw_95;
static Sint16 draw_94_polygons[223];
static Sint32 draw_94_vertices[123];
static NJS_CNK_MODEL draw_94_model;
static NJS_CNK_OBJECT draw_94;
static Sint16 draw_93_polygons[41];
static Sint32 draw_93_vertices[35];
static NJS_CNK_MODEL draw_93_model;
static NJS_CNK_OBJECT draw_93;
static Sint16 draw_92_polygons[67];
static Sint32 draw_92_vertices[51];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_92;
static Sint16 draw_91_polygons[53];
static Sint32 draw_91_vertices[51];
static NJS_CNK_MODEL draw_91_model;
static NJS_CNK_OBJECT draw_91;
static Sint16 draw_90_polygons[67];
static Sint32 draw_90_vertices[51];
static NJS_CNK_MODEL draw_90_model;
static NJS_CNK_OBJECT draw_90;
static Sint16 draw_89_polygons[235];
static Sint32 draw_89_vertices[119];
static NJS_CNK_MODEL draw_89_model;
static NJS_CNK_OBJECT draw_89;
static Sint16 draw_88_polygons[273];
static Sint32 draw_88_vertices[235];
static NJS_CNK_MODEL draw_88_model;
static NJS_CNK_OBJECT draw_88;
static Sint16 draw_85_polygons[1295];
static Sint32 draw_85_vertices[1151];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_87;
static Sint16 draw_83_polygons[115];
static Sint32 draw_83_vertices[111];
static NJS_CNK_MODEL draw_83_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_81_polygons[165];
static Sint32 draw_81_vertices[159];
static NJS_CNK_MODEL draw_81_model;
static NJS_CNK_OBJECT draw_82;
static Sint16 draw_75_polygons[449];
static Sint32 draw_75_vertices[275];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_63_polygons[73];
static Sint32 draw_63_vertices[75];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_60_polygons[145];
static Sint32 draw_60_vertices[87];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_48_polygons[29];
static Sint32 draw_48_vertices[27];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_45_polygons[29];
static Sint32 draw_45_vertices[27];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_44_polygons[121];
static Sint32 draw_44_vertices[67];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_42_polygons[121];
static Sint32 draw_42_vertices[67];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_41_polygons[85];
static Sint32 draw_41_vertices[51];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[85];
static Sint32 draw_40_vertices[51];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_37_polygons[85];
static Sint32 draw_37_vertices[51];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_25_polygons[101];
static Sint32 draw_25_vertices[99];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_13_polygons[55];
static Sint32 draw_13_vertices[43];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_12_polygons[329];
static Sint32 draw_12_vertices[275];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_4_polygons[169];
static Sint32 draw_4_vertices[163];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_11;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[142];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[70];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static Sint16 draw_0_polygons[1785];
static Sint32 draw_0_vertices[1659];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_3;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_44;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_97;
static NJS_OBJECT collision_98;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_100;
static NJS_OBJECT collision_102;
static NJS_OBJECT collision_103;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_108;
static NJS_OBJECT collision_109;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_111;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_113;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_118;
static NJS_OBJECT collision_121;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_123;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_129;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_131;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_133;
static NJS_OBJECT collision_134;
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_177;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
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
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
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
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
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
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
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
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[347] = {
#include "stg32/terrain/entries.inc"
};
struct land_table meteor_herd_land = {
#include "stg32/terrain/meteor_herd_land.inc"
};
static NJS_TEXNAME textures[55] = {
#include "stg32/terrain/textures.inc"
};
NJS_TEXLIST meteor_herd_land_texlist = {
#include "stg32/terrain/meteor_herd_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg32/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[197] = {
#include "stg32/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg32/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[86] = {
#include "stg32/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg32/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg32/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg32/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[64] = {
#include "stg32/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg32/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[33] = {
#include "stg32/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg32/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg32/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg32/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[50] = {
#include "stg32/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg32/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[24] = {
#include "stg32/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg32/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg32/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg32/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[171] = {
#include "stg32/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg32/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[78] = {
#include "stg32/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg32/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg32/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg32/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[79] = {
#include "stg32/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg32/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[38] = {
#include "stg32/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg32/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg32/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg32/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[170] = {
#include "stg32/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg32/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[77] = {
#include "stg32/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg32/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg32/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg32/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[27] = {
#include "stg32/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg32/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[16] = {
#include "stg32/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg32/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg32/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg32/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[166] = {
#include "stg32/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg32/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[75] = {
#include "stg32/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg32/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg32/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg32/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[19] = {
#include "stg32/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg32/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[16] = {
#include "stg32/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg32/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg32/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg32/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[74] = {
#include "stg32/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg32/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[34] = {
#include "stg32/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg32/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg32/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg32/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[40] = {
#include "stg32/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg32/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[18] = {
#include "stg32/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg32/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg32/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg32/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[19] = {
#include "stg32/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg32/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[11] = {
#include "stg32/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg32/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg32/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg32/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[252] = {
#include "stg32/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg32/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[108] = {
#include "stg32/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg32/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg32/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg32/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[7] = {
#include "stg32/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg32/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[6] = {
#include "stg32/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg32/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg32/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg32/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[60] = {
#include "stg32/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg32/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[36] = {
#include "stg32/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg32/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg32/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg32/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[42] = {
#include "stg32/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg32/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[16] = {
#include "stg32/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg32/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg32/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg32/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[60] = {
#include "stg32/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg32/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[36] = {
#include "stg32/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg32/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg32/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg32/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[14] = {
#include "stg32/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg32/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[10] = {
#include "stg32/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg32/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg32/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg32/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[23] = {
#include "stg32/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg32/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[20] = {
#include "stg32/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg32/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg32/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg32/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[25] = {
#include "stg32/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg32/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[24] = {
#include "stg32/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg32/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg32/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg32/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[14] = {
#include "stg32/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg32/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[12] = {
#include "stg32/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg32/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg32/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg32/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[49] = {
#include "stg32/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg32/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[24] = {
#include "stg32/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg32/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg32/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg32/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[78] = {
#include "stg32/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg32/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[40] = {
#include "stg32/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg32/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg32/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg32/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[14] = {
#include "stg32/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg32/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[8] = {
#include "stg32/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg32/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg32/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg32/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[15] = {
#include "stg32/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg32/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[14] = {
#include "stg32/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg32/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg32/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg32/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[40] = {
#include "stg32/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg32/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[27] = {
#include "stg32/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg32/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg32/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg32/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[140] = {
#include "stg32/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg32/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[88] = {
#include "stg32/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg32/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg32/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg32/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[7] = {
#include "stg32/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg32/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[6] = {
#include "stg32/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg32/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg32/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg32/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[21] = {
#include "stg32/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg32/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[12] = {
#include "stg32/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg32/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg32/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg32/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[52] = {
#include "stg32/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg32/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[21] = {
#include "stg32/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg32/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg32/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg32/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[37] = {
#include "stg32/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg32/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[24] = {
#include "stg32/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg32/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg32/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg32/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[44] = {
#include "stg32/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg32/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[32] = {
#include "stg32/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg32/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg32/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_66_materials[1] = {
#include "stg32/terrain/collision_66_materials.inc"
};
static Sint16 collision_66_faces[14] = {
#include "stg32/terrain/collision_66_faces.inc"
};
static NJS_MESHSET collision_66_meshsets[1] = {
#include "stg32/terrain/collision_66_meshsets.inc"
};
static NJS_POINT3 collision_66_points[12] = {
#include "stg32/terrain/collision_66_points.inc"
};
static NJS_MODEL collision_66_model = {
#include "stg32/terrain/collision_66_model.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg32/terrain/collision_66.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg32/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[58] = {
#include "stg32/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg32/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[26] = {
#include "stg32/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg32/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg32/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_68_materials[1] = {
#include "stg32/terrain/collision_68_materials.inc"
};
static Sint16 collision_68_faces[14] = {
#include "stg32/terrain/collision_68_faces.inc"
};
static NJS_MESHSET collision_68_meshsets[1] = {
#include "stg32/terrain/collision_68_meshsets.inc"
};
static NJS_POINT3 collision_68_points[8] = {
#include "stg32/terrain/collision_68_points.inc"
};
static NJS_MODEL collision_68_model = {
#include "stg32/terrain/collision_68_model.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg32/terrain/collision_68.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg32/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[24] = {
#include "stg32/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg32/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[22] = {
#include "stg32/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg32/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg32/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg32/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[25] = {
#include "stg32/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg32/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[20] = {
#include "stg32/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg32/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg32/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg32/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[140] = {
#include "stg32/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg32/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[88] = {
#include "stg32/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg32/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg32/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg32/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[7] = {
#include "stg32/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg32/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[6] = {
#include "stg32/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg32/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg32/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg32/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[21] = {
#include "stg32/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg32/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[12] = {
#include "stg32/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg32/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg32/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg32/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[42] = {
#include "stg32/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg32/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[24] = {
#include "stg32/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg32/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg32/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg32/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[47] = {
#include "stg32/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg32/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[34] = {
#include "stg32/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg32/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg32/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg32/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[14] = {
#include "stg32/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg32/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[12] = {
#include "stg32/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg32/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg32/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg32/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[33] = {
#include "stg32/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg32/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[20] = {
#include "stg32/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg32/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg32/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg32/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[7] = {
#include "stg32/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg32/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[6] = {
#include "stg32/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg32/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg32/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg32/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[18] = {
#include "stg32/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg32/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[12] = {
#include "stg32/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg32/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg32/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg32/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[14] = {
#include "stg32/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg32/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[12] = {
#include "stg32/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg32/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg32/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg32/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[7] = {
#include "stg32/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg32/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[6] = {
#include "stg32/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg32/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg32/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg32/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[14] = {
#include "stg32/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg32/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[9] = {
#include "stg32/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg32/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg32/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_89_materials[1] = {
#include "stg32/terrain/collision_89_materials.inc"
};
static Sint16 collision_89_faces[18] = {
#include "stg32/terrain/collision_89_faces.inc"
};
static NJS_MESHSET collision_89_meshsets[1] = {
#include "stg32/terrain/collision_89_meshsets.inc"
};
static NJS_POINT3 collision_89_points[12] = {
#include "stg32/terrain/collision_89_points.inc"
};
static NJS_MODEL collision_89_model = {
#include "stg32/terrain/collision_89_model.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg32/terrain/collision_89.inc"
};
static NJS_MATERIAL collision_90_materials[1] = {
#include "stg32/terrain/collision_90_materials.inc"
};
static Sint16 collision_90_faces[13] = {
#include "stg32/terrain/collision_90_faces.inc"
};
static NJS_MESHSET collision_90_meshsets[1] = {
#include "stg32/terrain/collision_90_meshsets.inc"
};
static NJS_POINT3 collision_90_points[8] = {
#include "stg32/terrain/collision_90_points.inc"
};
static NJS_MODEL collision_90_model = {
#include "stg32/terrain/collision_90_model.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg32/terrain/collision_90.inc"
};
static NJS_MATERIAL collision_91_materials[1] = {
#include "stg32/terrain/collision_91_materials.inc"
};
static Sint16 collision_91_faces[143] = {
#include "stg32/terrain/collision_91_faces.inc"
};
static NJS_MESHSET collision_91_meshsets[1] = {
#include "stg32/terrain/collision_91_meshsets.inc"
};
static NJS_POINT3 collision_91_points[72] = {
#include "stg32/terrain/collision_91_points.inc"
};
static NJS_MODEL collision_91_model = {
#include "stg32/terrain/collision_91_model.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg32/terrain/collision_91.inc"
};
static NJS_MATERIAL collision_92_materials[1] = {
#include "stg32/terrain/collision_92_materials.inc"
};
static Sint16 collision_92_faces[167] = {
#include "stg32/terrain/collision_92_faces.inc"
};
static NJS_MESHSET collision_92_meshsets[1] = {
#include "stg32/terrain/collision_92_meshsets.inc"
};
static NJS_POINT3 collision_92_points[64] = {
#include "stg32/terrain/collision_92_points.inc"
};
static NJS_MODEL collision_92_model = {
#include "stg32/terrain/collision_92_model.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg32/terrain/collision_92.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg32/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[90] = {
#include "stg32/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg32/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[40] = {
#include "stg32/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg32/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg32/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_101_materials[1] = {
#include "stg32/terrain/collision_101_materials.inc"
};
static Sint16 collision_101_faces[21] = {
#include "stg32/terrain/collision_101_faces.inc"
};
static NJS_MESHSET collision_101_meshsets[1] = {
#include "stg32/terrain/collision_101_meshsets.inc"
};
static NJS_POINT3 collision_101_points[11] = {
#include "stg32/terrain/collision_101_points.inc"
};
static NJS_MODEL collision_101_model = {
#include "stg32/terrain/collision_101_model.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg32/terrain/collision_101.inc"
};
static NJS_MATERIAL collision_104_materials[1] = {
#include "stg32/terrain/collision_104_materials.inc"
};
static Sint16 collision_104_faces[60] = {
#include "stg32/terrain/collision_104_faces.inc"
};
static NJS_MESHSET collision_104_meshsets[1] = {
#include "stg32/terrain/collision_104_meshsets.inc"
};
static NJS_POINT3 collision_104_points[36] = {
#include "stg32/terrain/collision_104_points.inc"
};
static NJS_MODEL collision_104_model = {
#include "stg32/terrain/collision_104_model.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg32/terrain/collision_104.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg32/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[134] = {
#include "stg32/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg32/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[49] = {
#include "stg32/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg32/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg32/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg32/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[7] = {
#include "stg32/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg32/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[6] = {
#include "stg32/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg32/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg32/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg32/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[30] = {
#include "stg32/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg32/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[21] = {
#include "stg32/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg32/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg32/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg32/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[35] = {
#include "stg32/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg32/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[34] = {
#include "stg32/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg32/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg32/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg32/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[70] = {
#include "stg32/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg32/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[51] = {
#include "stg32/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg32/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg32/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg32/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[35] = {
#include "stg32/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg32/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[34] = {
#include "stg32/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg32/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg32/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg32/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[35] = {
#include "stg32/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg32/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[34] = {
#include "stg32/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg32/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg32/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_126_materials[1] = {
#include "stg32/terrain/collision_126_materials.inc"
};
static Sint16 collision_126_faces[35] = {
#include "stg32/terrain/collision_126_faces.inc"
};
static NJS_MESHSET collision_126_meshsets[1] = {
#include "stg32/terrain/collision_126_meshsets.inc"
};
static NJS_POINT3 collision_126_points[34] = {
#include "stg32/terrain/collision_126_points.inc"
};
static NJS_MODEL collision_126_model = {
#include "stg32/terrain/collision_126_model.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg32/terrain/collision_126.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg32/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[14] = {
#include "stg32/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg32/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[9] = {
#include "stg32/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg32/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg32/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg32/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[60] = {
#include "stg32/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg32/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[36] = {
#include "stg32/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg32/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg32/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg32/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[17] = {
#include "stg32/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg32/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[15] = {
#include "stg32/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg32/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg32/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg32/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[12] = {
#include "stg32/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg32/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[8] = {
#include "stg32/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg32/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg32/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_138_materials[1] = {
#include "stg32/terrain/collision_138_materials.inc"
};
static Sint16 collision_138_faces[39] = {
#include "stg32/terrain/collision_138_faces.inc"
};
static NJS_MESHSET collision_138_meshsets[1] = {
#include "stg32/terrain/collision_138_meshsets.inc"
};
static NJS_POINT3 collision_138_points[28] = {
#include "stg32/terrain/collision_138_points.inc"
};
static NJS_MODEL collision_138_model = {
#include "stg32/terrain/collision_138_model.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg32/terrain/collision_138.inc"
};
static NJS_MATERIAL collision_139_materials[1] = {
#include "stg32/terrain/collision_139_materials.inc"
};
static Sint16 collision_139_faces[59] = {
#include "stg32/terrain/collision_139_faces.inc"
};
static NJS_MESHSET collision_139_meshsets[1] = {
#include "stg32/terrain/collision_139_meshsets.inc"
};
static NJS_POINT3 collision_139_points[33] = {
#include "stg32/terrain/collision_139_points.inc"
};
static NJS_MODEL collision_139_model = {
#include "stg32/terrain/collision_139_model.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg32/terrain/collision_139.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg32/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[11] = {
#include "stg32/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg32/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[10] = {
#include "stg32/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg32/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg32/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_141_materials[1] = {
#include "stg32/terrain/collision_141_materials.inc"
};
static Sint16 collision_141_faces[51] = {
#include "stg32/terrain/collision_141_faces.inc"
};
static NJS_MESHSET collision_141_meshsets[1] = {
#include "stg32/terrain/collision_141_meshsets.inc"
};
static NJS_POINT3 collision_141_points[31] = {
#include "stg32/terrain/collision_141_points.inc"
};
static NJS_MODEL collision_141_model = {
#include "stg32/terrain/collision_141_model.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg32/terrain/collision_141.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg32/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[12] = {
#include "stg32/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg32/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[8] = {
#include "stg32/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg32/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg32/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg32/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[18] = {
#include "stg32/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg32/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[16] = {
#include "stg32/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg32/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg32/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg32/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[11] = {
#include "stg32/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg32/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[10] = {
#include "stg32/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg32/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg32/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_145_materials[1] = {
#include "stg32/terrain/collision_145_materials.inc"
};
static Sint16 collision_145_faces[23] = {
#include "stg32/terrain/collision_145_faces.inc"
};
static NJS_MESHSET collision_145_meshsets[1] = {
#include "stg32/terrain/collision_145_meshsets.inc"
};
static NJS_POINT3 collision_145_points[15] = {
#include "stg32/terrain/collision_145_points.inc"
};
static NJS_MODEL collision_145_model = {
#include "stg32/terrain/collision_145_model.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg32/terrain/collision_145.inc"
};
static Sint16 draw_196_polygons[43] = {
#include "stg32/terrain/draw_196_polygons.inc"
};
static Sint32 draw_196_vertices[35] = {
#include "stg32/terrain/draw_196_vertices.inc"
};
static NJS_CNK_MODEL draw_196_model = {
#include "stg32/terrain/draw_196_model.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg32/terrain/draw_196.inc"
};
static Sint16 draw_194_polygons[257] = {
#include "stg32/terrain/draw_194_polygons.inc"
};
static Sint32 draw_194_vertices[187] = {
#include "stg32/terrain/draw_194_vertices.inc"
};
static NJS_CNK_MODEL draw_194_model = {
#include "stg32/terrain/draw_194_model.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg32/terrain/draw_195.inc"
};
static Sint16 draw_192_polygons[125] = {
#include "stg32/terrain/draw_192_polygons.inc"
};
static Sint32 draw_192_vertices[99] = {
#include "stg32/terrain/draw_192_vertices.inc"
};
static NJS_CNK_MODEL draw_192_model = {
#include "stg32/terrain/draw_192_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg32/terrain/draw_193.inc"
};
static Sint16 draw_188_polygons[55] = {
#include "stg32/terrain/draw_188_polygons.inc"
};
static Sint32 draw_188_vertices[51] = {
#include "stg32/terrain/draw_188_vertices.inc"
};
static NJS_CNK_MODEL draw_188_model = {
#include "stg32/terrain/draw_188_model.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg32/terrain/draw_191.inc"
};
static Sint16 draw_187_polygons[71] = {
#include "stg32/terrain/draw_187_polygons.inc"
};
static Sint32 draw_187_vertices[75] = {
#include "stg32/terrain/draw_187_vertices.inc"
};
static NJS_CNK_MODEL draw_187_model = {
#include "stg32/terrain/draw_187_model.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg32/terrain/draw_187.inc"
};
static Sint16 draw_186_polygons[347] = {
#include "stg32/terrain/draw_186_polygons.inc"
};
static Sint32 draw_186_vertices[195] = {
#include "stg32/terrain/draw_186_vertices.inc"
};
static NJS_CNK_MODEL draw_186_model = {
#include "stg32/terrain/draw_186_model.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg32/terrain/draw_186.inc"
};
static Sint16 draw_185_polygons[157] = {
#include "stg32/terrain/draw_185_polygons.inc"
};
static Sint32 draw_185_vertices[107] = {
#include "stg32/terrain/draw_185_vertices.inc"
};
static NJS_CNK_MODEL draw_185_model = {
#include "stg32/terrain/draw_185_model.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg32/terrain/draw_185.inc"
};
static Sint16 draw_184_polygons[57] = {
#include "stg32/terrain/draw_184_polygons.inc"
};
static Sint32 draw_184_vertices[55] = {
#include "stg32/terrain/draw_184_vertices.inc"
};
static NJS_CNK_MODEL draw_184_model = {
#include "stg32/terrain/draw_184_model.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg32/terrain/draw_184.inc"
};
static Sint16 draw_181_polygons[225] = {
#include "stg32/terrain/draw_181_polygons.inc"
};
static Sint32 draw_181_vertices[163] = {
#include "stg32/terrain/draw_181_vertices.inc"
};
static NJS_CNK_MODEL draw_181_model = {
#include "stg32/terrain/draw_181_model.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg32/terrain/draw_183.inc"
};
static Sint16 draw_180_polygons[205] = {
#include "stg32/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[195] = {
#include "stg32/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg32/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg32/terrain/draw_180.inc"
};
static Sint16 draw_179_polygons[129] = {
#include "stg32/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[139] = {
#include "stg32/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg32/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg32/terrain/draw_179.inc"
};
static Sint16 draw_175_polygons[473] = {
#include "stg32/terrain/draw_175_polygons.inc"
};
static Sint32 draw_175_vertices[323] = {
#include "stg32/terrain/draw_175_vertices.inc"
};
static NJS_CNK_MODEL draw_175_model = {
#include "stg32/terrain/draw_175_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg32/terrain/draw_178.inc"
};
static Sint16 draw_171_polygons[55] = {
#include "stg32/terrain/draw_171_polygons.inc"
};
static Sint32 draw_171_vertices[43] = {
#include "stg32/terrain/draw_171_vertices.inc"
};
static NJS_CNK_MODEL draw_171_model = {
#include "stg32/terrain/draw_171_model.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg32/terrain/draw_174.inc"
};
static Sint16 draw_170_polygons[47] = {
#include "stg32/terrain/draw_170_polygons.inc"
};
static Sint32 draw_170_vertices[51] = {
#include "stg32/terrain/draw_170_vertices.inc"
};
static NJS_CNK_MODEL draw_170_model = {
#include "stg32/terrain/draw_170_model.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg32/terrain/draw_170.inc"
};
static Sint16 draw_169_polygons[213] = {
#include "stg32/terrain/draw_169_polygons.inc"
};
static Sint32 draw_169_vertices[147] = {
#include "stg32/terrain/draw_169_vertices.inc"
};
static NJS_CNK_MODEL draw_169_model = {
#include "stg32/terrain/draw_169_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg32/terrain/draw_169.inc"
};
static Sint16 draw_168_polygons[211] = {
#include "stg32/terrain/draw_168_polygons.inc"
};
static Sint32 draw_168_vertices[127] = {
#include "stg32/terrain/draw_168_vertices.inc"
};
static NJS_CNK_MODEL draw_168_model = {
#include "stg32/terrain/draw_168_model.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg32/terrain/draw_168.inc"
};
static Sint16 draw_167_polygons[37] = {
#include "stg32/terrain/draw_167_polygons.inc"
};
static Sint32 draw_167_vertices[35] = {
#include "stg32/terrain/draw_167_vertices.inc"
};
static NJS_CNK_MODEL draw_167_model = {
#include "stg32/terrain/draw_167_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg32/terrain/draw_167.inc"
};
static Sint16 draw_155_polygons[157] = {
#include "stg32/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[139] = {
#include "stg32/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg32/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg32/terrain/draw_166.inc"
};
static Sint16 draw_154_polygons[67] = {
#include "stg32/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[63] = {
#include "stg32/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg32/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg32/terrain/draw_154.inc"
};
static Sint16 draw_153_polygons[35] = {
#include "stg32/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[35] = {
#include "stg32/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg32/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg32/terrain/draw_153.inc"
};
static Sint16 draw_151_polygons[167] = {
#include "stg32/terrain/draw_151_polygons.inc"
};
static Sint32 draw_151_vertices[103] = {
#include "stg32/terrain/draw_151_vertices.inc"
};
static NJS_CNK_MODEL draw_151_model = {
#include "stg32/terrain/draw_151_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg32/terrain/draw_152.inc"
};
static Sint16 draw_150_polygons[55] = {
#include "stg32/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[51] = {
#include "stg32/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg32/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg32/terrain/draw_150.inc"
};
static Sint16 draw_149_polygons[71] = {
#include "stg32/terrain/draw_149_polygons.inc"
};
static Sint32 draw_149_vertices[75] = {
#include "stg32/terrain/draw_149_vertices.inc"
};
static NJS_CNK_MODEL draw_149_model = {
#include "stg32/terrain/draw_149_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg32/terrain/draw_149.inc"
};
static Sint16 draw_148_polygons[89] = {
#include "stg32/terrain/draw_148_polygons.inc"
};
static Sint32 draw_148_vertices[67] = {
#include "stg32/terrain/draw_148_vertices.inc"
};
static NJS_CNK_MODEL draw_148_model = {
#include "stg32/terrain/draw_148_model.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg32/terrain/draw_148.inc"
};
static Sint16 draw_146_polygons[115] = {
#include "stg32/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[103] = {
#include "stg32/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg32/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg32/terrain/draw_147.inc"
};
static Sint16 draw_143_polygons[111] = {
#include "stg32/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[83] = {
#include "stg32/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg32/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg32/terrain/draw_145.inc"
};
static Sint16 draw_141_polygons[357] = {
#include "stg32/terrain/draw_141_polygons.inc"
};
static Sint32 draw_141_vertices[247] = {
#include "stg32/terrain/draw_141_vertices.inc"
};
static NJS_CNK_MODEL draw_141_model = {
#include "stg32/terrain/draw_141_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg32/terrain/draw_142.inc"
};
static Sint16 draw_140_polygons[163] = {
#include "stg32/terrain/draw_140_polygons.inc"
};
static Sint32 draw_140_vertices[195] = {
#include "stg32/terrain/draw_140_vertices.inc"
};
static NJS_CNK_MODEL draw_140_model = {
#include "stg32/terrain/draw_140_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg32/terrain/draw_140.inc"
};
static Sint16 draw_139_polygons[579] = {
#include "stg32/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[411] = {
#include "stg32/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg32/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg32/terrain/draw_139.inc"
};
static Sint16 draw_135_polygons[103] = {
#include "stg32/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[83] = {
#include "stg32/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg32/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg32/terrain/draw_138.inc"
};
static Sint16 draw_134_polygons[127] = {
#include "stg32/terrain/draw_134_polygons.inc"
};
static Sint32 draw_134_vertices[99] = {
#include "stg32/terrain/draw_134_vertices.inc"
};
static NJS_CNK_MODEL draw_134_model = {
#include "stg32/terrain/draw_134_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg32/terrain/draw_134.inc"
};
static Sint16 draw_133_polygons[261] = {
#include "stg32/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[147] = {
#include "stg32/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg32/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg32/terrain/draw_133.inc"
};
static Sint16 draw_132_polygons[61] = {
#include "stg32/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[67] = {
#include "stg32/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg32/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg32/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[49] = {
#include "stg32/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[51] = {
#include "stg32/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg32/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg32/terrain/draw_131.inc"
};
static Sint16 draw_119_polygons[101] = {
#include "stg32/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[95] = {
#include "stg32/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg32/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg32/terrain/draw_130.inc"
};
static Sint16 draw_118_polygons[67] = {
#include "stg32/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[51] = {
#include "stg32/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg32/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg32/terrain/draw_118.inc"
};
static Sint16 draw_117_polygons[61] = {
#include "stg32/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[67] = {
#include "stg32/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg32/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg32/terrain/draw_117.inc"
};
static Sint16 draw_116_polygons[55] = {
#include "stg32/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[51] = {
#include "stg32/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg32/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg32/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[71] = {
#include "stg32/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[75] = {
#include "stg32/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg32/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg32/terrain/draw_115.inc"
};
static Sint16 draw_114_polygons[75] = {
#include "stg32/terrain/draw_114_polygons.inc"
};
static Sint32 draw_114_vertices[51] = {
#include "stg32/terrain/draw_114_vertices.inc"
};
static NJS_CNK_MODEL draw_114_model = {
#include "stg32/terrain/draw_114_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg32/terrain/draw_114.inc"
};
static Sint16 draw_112_polygons[165] = {
#include "stg32/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[159] = {
#include "stg32/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg32/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg32/terrain/draw_113.inc"
};
static Sint16 draw_108_polygons[187] = {
#include "stg32/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[139] = {
#include "stg32/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg32/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg32/terrain/draw_111.inc"
};
static Sint16 draw_107_polygons[185] = {
#include "stg32/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[195] = {
#include "stg32/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg32/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg32/terrain/draw_107.inc"
};
static Sint16 draw_106_polygons[125] = {
#include "stg32/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[99] = {
#include "stg32/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg32/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg32/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[61] = {
#include "stg32/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[67] = {
#include "stg32/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg32/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg32/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[37] = {
#include "stg32/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[35] = {
#include "stg32/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg32/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg32/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[55] = {
#include "stg32/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[51] = {
#include "stg32/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg32/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg32/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[41] = {
#include "stg32/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[35] = {
#include "stg32/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg32/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg32/terrain/draw_102.inc"
};
static Sint16 draw_101_polygons[103] = {
#include "stg32/terrain/draw_101_polygons.inc"
};
static Sint32 draw_101_vertices[75] = {
#include "stg32/terrain/draw_101_vertices.inc"
};
static NJS_CNK_MODEL draw_101_model = {
#include "stg32/terrain/draw_101_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg32/terrain/draw_101.inc"
};
static Sint16 draw_100_polygons[187] = {
#include "stg32/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[203] = {
#include "stg32/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg32/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg32/terrain/draw_100.inc"
};
static Sint16 draw_99_polygons[211] = {
#include "stg32/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[131] = {
#include "stg32/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg32/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg32/terrain/draw_99.inc"
};
static Sint16 draw_98_polygons[51] = {
#include "stg32/terrain/draw_98_polygons.inc"
};
static Sint32 draw_98_vertices[51] = {
#include "stg32/terrain/draw_98_vertices.inc"
};
static NJS_CNK_MODEL draw_98_model = {
#include "stg32/terrain/draw_98_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg32/terrain/draw_98.inc"
};
static Sint16 draw_97_polygons[55] = {
#include "stg32/terrain/draw_97_polygons.inc"
};
static Sint32 draw_97_vertices[51] = {
#include "stg32/terrain/draw_97_vertices.inc"
};
static NJS_CNK_MODEL draw_97_model = {
#include "stg32/terrain/draw_97_model.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg32/terrain/draw_97.inc"
};
static Sint16 draw_96_polygons[47] = {
#include "stg32/terrain/draw_96_polygons.inc"
};
static Sint32 draw_96_vertices[43] = {
#include "stg32/terrain/draw_96_vertices.inc"
};
static NJS_CNK_MODEL draw_96_model = {
#include "stg32/terrain/draw_96_model.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg32/terrain/draw_96.inc"
};
static Sint16 draw_95_polygons[171] = {
#include "stg32/terrain/draw_95_polygons.inc"
};
static Sint32 draw_95_vertices[195] = {
#include "stg32/terrain/draw_95_vertices.inc"
};
static NJS_CNK_MODEL draw_95_model = {
#include "stg32/terrain/draw_95_model.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg32/terrain/draw_95.inc"
};
static Sint16 draw_94_polygons[223] = {
#include "stg32/terrain/draw_94_polygons.inc"
};
static Sint32 draw_94_vertices[123] = {
#include "stg32/terrain/draw_94_vertices.inc"
};
static NJS_CNK_MODEL draw_94_model = {
#include "stg32/terrain/draw_94_model.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg32/terrain/draw_94.inc"
};
static Sint16 draw_93_polygons[41] = {
#include "stg32/terrain/draw_93_polygons.inc"
};
static Sint32 draw_93_vertices[35] = {
#include "stg32/terrain/draw_93_vertices.inc"
};
static NJS_CNK_MODEL draw_93_model = {
#include "stg32/terrain/draw_93_model.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg32/terrain/draw_93.inc"
};
static Sint16 draw_92_polygons[67] = {
#include "stg32/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[51] = {
#include "stg32/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg32/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg32/terrain/draw_92.inc"
};
static Sint16 draw_91_polygons[53] = {
#include "stg32/terrain/draw_91_polygons.inc"
};
static Sint32 draw_91_vertices[51] = {
#include "stg32/terrain/draw_91_vertices.inc"
};
static NJS_CNK_MODEL draw_91_model = {
#include "stg32/terrain/draw_91_model.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg32/terrain/draw_91.inc"
};
static Sint16 draw_90_polygons[67] = {
#include "stg32/terrain/draw_90_polygons.inc"
};
static Sint32 draw_90_vertices[51] = {
#include "stg32/terrain/draw_90_vertices.inc"
};
static NJS_CNK_MODEL draw_90_model = {
#include "stg32/terrain/draw_90_model.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg32/terrain/draw_90.inc"
};
static Sint16 draw_89_polygons[235] = {
#include "stg32/terrain/draw_89_polygons.inc"
};
static Sint32 draw_89_vertices[119] = {
#include "stg32/terrain/draw_89_vertices.inc"
};
static NJS_CNK_MODEL draw_89_model = {
#include "stg32/terrain/draw_89_model.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg32/terrain/draw_89.inc"
};
static Sint16 draw_88_polygons[273] = {
#include "stg32/terrain/draw_88_polygons.inc"
};
static Sint32 draw_88_vertices[235] = {
#include "stg32/terrain/draw_88_vertices.inc"
};
static NJS_CNK_MODEL draw_88_model = {
#include "stg32/terrain/draw_88_model.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg32/terrain/draw_88.inc"
};
static Sint16 draw_85_polygons[1295] = {
#include "stg32/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[1151] = {
#include "stg32/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg32/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg32/terrain/draw_87.inc"
};
static Sint16 draw_83_polygons[115] = {
#include "stg32/terrain/draw_83_polygons.inc"
};
static Sint32 draw_83_vertices[111] = {
#include "stg32/terrain/draw_83_vertices.inc"
};
static NJS_CNK_MODEL draw_83_model = {
#include "stg32/terrain/draw_83_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg32/terrain/draw_84.inc"
};
static Sint16 draw_81_polygons[165] = {
#include "stg32/terrain/draw_81_polygons.inc"
};
static Sint32 draw_81_vertices[159] = {
#include "stg32/terrain/draw_81_vertices.inc"
};
static NJS_CNK_MODEL draw_81_model = {
#include "stg32/terrain/draw_81_model.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg32/terrain/draw_82.inc"
};
static Sint16 draw_75_polygons[449] = {
#include "stg32/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[275] = {
#include "stg32/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg32/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg32/terrain/draw_80.inc"
};
static Sint16 draw_63_polygons[73] = {
#include "stg32/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[75] = {
#include "stg32/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg32/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg32/terrain/draw_74.inc"
};
static Sint16 draw_60_polygons[145] = {
#include "stg32/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[87] = {
#include "stg32/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg32/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg32/terrain/draw_62.inc"
};
static Sint16 draw_48_polygons[29] = {
#include "stg32/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[27] = {
#include "stg32/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg32/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg32/terrain/draw_59.inc"
};
static Sint16 draw_45_polygons[29] = {
#include "stg32/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[27] = {
#include "stg32/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg32/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg32/terrain/draw_47.inc"
};
static Sint16 draw_44_polygons[121] = {
#include "stg32/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[67] = {
#include "stg32/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg32/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg32/terrain/draw_44.inc"
};
static Sint16 draw_42_polygons[121] = {
#include "stg32/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[67] = {
#include "stg32/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg32/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg32/terrain/draw_43.inc"
};
static Sint16 draw_41_polygons[85] = {
#include "stg32/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[51] = {
#include "stg32/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg32/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg32/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[85] = {
#include "stg32/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[51] = {
#include "stg32/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg32/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg32/terrain/draw_40.inc"
};
static Sint16 draw_37_polygons[85] = {
#include "stg32/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[51] = {
#include "stg32/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg32/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg32/terrain/draw_39.inc"
};
static Sint16 draw_25_polygons[101] = {
#include "stg32/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[99] = {
#include "stg32/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg32/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg32/terrain/draw_36.inc"
};
static Sint16 draw_13_polygons[55] = {
#include "stg32/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[43] = {
#include "stg32/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg32/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg32/terrain/draw_24.inc"
};
static Sint16 draw_12_polygons[329] = {
#include "stg32/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[275] = {
#include "stg32/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg32/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg32/terrain/draw_12.inc"
};
static Sint16 draw_4_polygons[169] = {
#include "stg32/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[163] = {
#include "stg32/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg32/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg32/terrain/draw_11.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg32/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[142] = {
#include "stg32/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg32/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[70] = {
#include "stg32/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg32/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg32/terrain/collision_146.inc"
};
static Sint16 draw_0_polygons[1785] = {
#include "stg32/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[1659] = {
#include "stg32/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg32/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg32/terrain/draw_3.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg32/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg32/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg32/terrain/collision_9.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg32/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg32/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg32/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg32/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg32/terrain/collision_16.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg32/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg32/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg32/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg32/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg32/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg32/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg32/terrain/collision_24.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg32/terrain/collision_26.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg32/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg32/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg32/terrain/collision_32.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg32/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg32/terrain/collision_36.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg32/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg32/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg32/terrain/collision_41.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg32/terrain/collision_43.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg32/terrain/collision_44.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg32/terrain/collision_45.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg32/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg32/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg32/terrain/collision_52.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg32/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg32/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg32/terrain/collision_57.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg32/terrain/collision_63.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg32/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg32/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg32/terrain/collision_77.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg32/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg32/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg32/terrain/collision_83.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg32/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg32/terrain/collision_94.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg32/terrain/collision_96.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg32/terrain/collision_97.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg32/terrain/collision_98.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg32/terrain/collision_99.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg32/terrain/collision_100.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg32/terrain/collision_102.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg32/terrain/collision_103.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg32/terrain/collision_105.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg32/terrain/collision_108.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg32/terrain/collision_109.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg32/terrain/collision_110.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg32/terrain/collision_111.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg32/terrain/collision_112.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg32/terrain/collision_113.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg32/terrain/collision_114.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg32/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg32/terrain/collision_117.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg32/terrain/collision_118.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg32/terrain/collision_121.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg32/terrain/collision_122.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg32/terrain/collision_123.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg32/terrain/collision_128.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg32/terrain/collision_129.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg32/terrain/collision_130.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg32/terrain/collision_131.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg32/terrain/collision_132.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg32/terrain/collision_133.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg32/terrain/collision_134.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg32/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg32/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg32/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg32/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg32/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg32/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg32/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg32/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg32/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg32/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg32/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg32/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg32/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg32/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg32/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg32/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg32/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg32/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg32/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg32/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg32/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg32/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg32/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg32/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg32/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg32/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg32/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg32/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg32/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg32/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg32/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg32/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg32/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg32/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg32/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg32/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg32/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg32/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg32/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg32/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg32/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg32/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg32/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg32/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg32/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg32/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg32/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg32/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg32/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg32/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg32/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg32/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg32/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg32/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg32/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg32/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg32/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg32/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg32/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg32/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg32/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg32/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg32/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg32/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg32/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg32/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg32/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg32/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg32/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg32/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg32/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg32/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg32/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg32/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg32/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg32/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg32/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg32/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg32/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg32/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg32/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg32/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg32/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg32/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg32/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg32/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg32/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg32/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg32/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg32/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg32/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg32/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg32/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg32/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg32/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg32/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg32/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg32/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg32/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg32/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg32/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg32/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg32/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg32/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg32/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg32/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg32/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg32/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg32/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg32/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg32/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg32/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg32/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg32/terrain/draw_4.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg32/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg32/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg32/terrain/collision_149.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg32/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg32/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg32/terrain/draw_0.inc"
};
