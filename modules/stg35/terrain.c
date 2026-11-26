/* STG35.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between background_archives.c and
 * object_archives.c, and its D lies between machine_lights.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg35/core_eggman.h"
#include "stg35/machinery.h"

#include "stg35/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[295];
struct land_table core_eggman_land;
static NJS_TEXNAME textures[32];
NJS_TEXLIST eggman_glow_texlist;
static Sint16 draw_197_polygons[871];
static Sint32 draw_197_vertices[675];
static NJS_CNK_MODEL draw_197_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_147_polygons[121];
static Sint32 draw_147_vertices[115];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_193;
static Sint16 draw_173_polygons[315];
static Sint32 draw_173_vertices[351];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_121_polygons[293];
static Sint32 draw_121_vertices[231];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_113_polygons[121];
static Sint32 draw_113_vertices[115];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_106_polygons[199];
static Sint32 draw_106_vertices[163];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_105_polygons[155];
static Sint32 draw_105_vertices[163];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_85_polygons[1271];
static Sint32 draw_85_vertices[1059];
static NJS_CNK_MODEL draw_85_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_75_polygons[99];
static Sint32 draw_75_vertices[83];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_84;
static Sint16 draw_68_polygons[1105];
static Sint32 draw_68_vertices[1171];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_66_polygons[129];
static Sint32 draw_66_vertices[99];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_63_polygons[417];
static Sint32 draw_63_vertices[255];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_62_polygons[155];
static Sint32 draw_62_vertices[163];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[115];
static Sint32 draw_61_vertices[83];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_59_polygons[1271];
static Sint32 draw_59_vertices[1059];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_58_polygons[155];
static Sint32 draw_58_vertices[163];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[403];
static Sint32 draw_57_vertices[419];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_47_polygons[455];
static Sint32 draw_47_vertices[483];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_45_polygons[455];
static Sint32 draw_45_vertices[483];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_44_polygons[65];
static Sint32 draw_44_vertices[67];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[155];
static Sint32 draw_43_vertices[163];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[155];
static Sint32 draw_42_vertices[163];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[155];
static Sint32 draw_41_vertices[99];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[1661];
static Sint32 draw_40_vertices[1331];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[67];
static Sint32 draw_39_vertices[75];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[2453];
static Sint32 draw_38_vertices[1419];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[2381];
static Sint32 draw_37_vertices[1407];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[2381];
static Sint32 draw_36_vertices[1407];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_34_polygons[127];
static Sint32 draw_34_vertices[127];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_32_polygons[1213];
static Sint32 draw_32_vertices[1203];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[1213];
static Sint32 draw_31_vertices[1203];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[91];
static Sint32 draw_30_vertices[99];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[99];
static Sint32 draw_29_vertices[83];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[929];
static Sint32 draw_28_vertices[579];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[1779];
static Sint32 draw_27_vertices[1403];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[883];
static Sint32 draw_26_vertices[483];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[135];
static Sint32 draw_25_vertices[147];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[141];
static Sint32 draw_24_vertices[163];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[229];
static Sint32 draw_23_vertices[171];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[593];
static Sint32 draw_22_vertices[723];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[893];
static Sint32 draw_21_vertices[827];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_16_polygons[709];
static Sint32 draw_16_vertices[627];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_14_polygons[723];
static Sint32 draw_14_vertices[691];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_13_polygons[243];
static Sint32 draw_13_vertices[219];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[155];
static Sint32 draw_12_vertices[179];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[65];
static Sint32 draw_11_vertices[67];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[685];
static Sint32 draw_10_vertices[379];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[949];
static Sint32 draw_9_vertices[1155];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[545];
static Sint32 draw_8_vertices[475];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[147];
static Sint32 draw_7_vertices[111];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[1065];
static Sint32 draw_6_vertices[747];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[25];
static Sint32 draw_5_vertices[19];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[25];
static Sint32 draw_4_vertices[19];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[25];
static Sint32 draw_3_vertices[19];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[25];
static Sint32 draw_2_vertices[19];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[199];
static Sint32 draw_1_vertices[111];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[115];
static Sint32 draw_0_vertices[87];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[36];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[22];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[401];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[188];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[166];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[72];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[79];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[33];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[136];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[68];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[36];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[22];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[14];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[12];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[38];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[24];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[10];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[8];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[14];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[9];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[15];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[8];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[42];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[21];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[33];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[21];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[52];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[24];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[20];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[12];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[56];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[25];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[20];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[10];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[19];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[10];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[73];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[48];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[133];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[60];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[133];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[60];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[133];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[60];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[144];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[54];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_78_materials[1];
static Sint16 collision_78_faces[16];
static NJS_MESHSET collision_78_meshsets[1];
static NJS_POINT3 collision_78_points[8];
static NJS_MODEL collision_78_model;
static NJS_OBJECT collision_78;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[16];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[8];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_80_materials[1];
static Sint16 collision_80_faces[16];
static NJS_MESHSET collision_80_meshsets[1];
static NJS_POINT3 collision_80_points[8];
static NJS_MODEL collision_80_model;
static NJS_OBJECT collision_80;
static NJS_MATERIAL collision_81_materials[1];
static Sint16 collision_81_faces[16];
static NJS_MESHSET collision_81_meshsets[1];
static NJS_POINT3 collision_81_points[8];
static NJS_MODEL collision_81_model;
static NJS_OBJECT collision_81;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[16];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[8];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[16];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[8];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[16];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[8];
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
static NJS_POINT3 collision_86_points[9];
static NJS_MODEL collision_86_model;
static NJS_OBJECT collision_86;
static NJS_MATERIAL collision_87_materials[1];
static Sint16 collision_87_faces[30];
static NJS_MESHSET collision_87_meshsets[1];
static NJS_POINT3 collision_87_points[21];
static NJS_MODEL collision_87_model;
static NJS_OBJECT collision_87;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[18];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[12];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_203;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_184;
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
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_145;
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
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
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
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_99;
static NJS_CNK_OBJECT draw_98;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_80;
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
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_14;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_55;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_57;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;

static struct land_entry entries[295] = {
#include "stg35/terrain/entries.inc"
};
struct land_table core_eggman_land = {
#include "stg35/terrain/core_eggman_land.inc"
};
static NJS_TEXNAME textures[32] = {
#include "stg35/terrain/textures.inc"
};
NJS_TEXLIST eggman_glow_texlist = {
#include "stg35/terrain/eggman_glow_texlist.inc"
};
static Sint16 draw_197_polygons[871] = {
#include "stg35/terrain/draw_197_polygons.inc"
};
static Sint32 draw_197_vertices[675] = {
#include "stg35/terrain/draw_197_vertices.inc"
};
static NJS_CNK_MODEL draw_197_model = {
#include "stg35/terrain/draw_197_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg35/terrain/draw_199.inc"
};
static Sint16 draw_147_polygons[121] = {
#include "stg35/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[115] = {
#include "stg35/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg35/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg35/terrain/draw_193.inc"
};
static Sint16 draw_173_polygons[315] = {
#include "stg35/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[351] = {
#include "stg35/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg35/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg35/terrain/draw_173.inc"
};
static Sint16 draw_121_polygons[293] = {
#include "stg35/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[231] = {
#include "stg35/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg35/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg35/terrain/draw_146.inc"
};
static Sint16 draw_113_polygons[121] = {
#include "stg35/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[115] = {
#include "stg35/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg35/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg35/terrain/draw_116.inc"
};
static Sint16 draw_106_polygons[199] = {
#include "stg35/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[163] = {
#include "stg35/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg35/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg35/terrain/draw_112.inc"
};
static Sint16 draw_105_polygons[155] = {
#include "stg35/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[163] = {
#include "stg35/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg35/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg35/terrain/draw_105.inc"
};
static Sint16 draw_85_polygons[1271] = {
#include "stg35/terrain/draw_85_polygons.inc"
};
static Sint32 draw_85_vertices[1059] = {
#include "stg35/terrain/draw_85_vertices.inc"
};
static NJS_CNK_MODEL draw_85_model = {
#include "stg35/terrain/draw_85_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg35/terrain/draw_104.inc"
};
static Sint16 draw_75_polygons[99] = {
#include "stg35/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[83] = {
#include "stg35/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg35/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg35/terrain/draw_84.inc"
};
static Sint16 draw_68_polygons[1105] = {
#include "stg35/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[1171] = {
#include "stg35/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg35/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg35/terrain/draw_74.inc"
};
static Sint16 draw_66_polygons[129] = {
#include "stg35/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[99] = {
#include "stg35/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg35/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg35/terrain/draw_67.inc"
};
static Sint16 draw_63_polygons[417] = {
#include "stg35/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[255] = {
#include "stg35/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg35/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg35/terrain/draw_65.inc"
};
static Sint16 draw_62_polygons[155] = {
#include "stg35/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[163] = {
#include "stg35/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg35/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg35/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[115] = {
#include "stg35/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[83] = {
#include "stg35/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg35/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg35/terrain/draw_61.inc"
};
static Sint16 draw_59_polygons[1271] = {
#include "stg35/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[1059] = {
#include "stg35/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg35/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg35/terrain/draw_60.inc"
};
static Sint16 draw_58_polygons[155] = {
#include "stg35/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[163] = {
#include "stg35/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg35/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg35/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[403] = {
#include "stg35/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[419] = {
#include "stg35/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg35/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg35/terrain/draw_57.inc"
};
static Sint16 draw_47_polygons[455] = {
#include "stg35/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[483] = {
#include "stg35/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg35/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg35/terrain/draw_55.inc"
};
static Sint16 draw_45_polygons[455] = {
#include "stg35/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[483] = {
#include "stg35/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg35/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg35/terrain/draw_46.inc"
};
static Sint16 draw_44_polygons[65] = {
#include "stg35/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[67] = {
#include "stg35/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg35/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg35/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[155] = {
#include "stg35/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[163] = {
#include "stg35/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg35/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg35/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[155] = {
#include "stg35/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[163] = {
#include "stg35/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg35/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg35/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[155] = {
#include "stg35/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[99] = {
#include "stg35/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg35/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg35/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[1661] = {
#include "stg35/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[1331] = {
#include "stg35/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg35/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg35/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[67] = {
#include "stg35/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[75] = {
#include "stg35/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg35/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg35/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[2453] = {
#include "stg35/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[1419] = {
#include "stg35/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg35/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg35/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[2381] = {
#include "stg35/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[1407] = {
#include "stg35/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg35/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg35/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[2381] = {
#include "stg35/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[1407] = {
#include "stg35/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg35/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg35/terrain/draw_36.inc"
};
static Sint16 draw_34_polygons[127] = {
#include "stg35/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[127] = {
#include "stg35/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg35/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg35/terrain/draw_35.inc"
};
static Sint16 draw_32_polygons[1213] = {
#include "stg35/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[1203] = {
#include "stg35/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg35/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg35/terrain/draw_33.inc"
};
static Sint16 draw_31_polygons[1213] = {
#include "stg35/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[1203] = {
#include "stg35/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg35/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg35/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[91] = {
#include "stg35/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[99] = {
#include "stg35/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg35/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg35/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[99] = {
#include "stg35/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[83] = {
#include "stg35/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg35/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg35/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[929] = {
#include "stg35/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[579] = {
#include "stg35/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg35/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg35/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[1779] = {
#include "stg35/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[1403] = {
#include "stg35/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg35/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg35/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[883] = {
#include "stg35/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[483] = {
#include "stg35/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg35/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg35/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[135] = {
#include "stg35/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[147] = {
#include "stg35/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg35/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg35/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[141] = {
#include "stg35/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[163] = {
#include "stg35/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg35/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg35/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[229] = {
#include "stg35/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[171] = {
#include "stg35/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg35/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg35/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[593] = {
#include "stg35/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[723] = {
#include "stg35/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg35/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg35/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[893] = {
#include "stg35/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[827] = {
#include "stg35/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg35/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg35/terrain/draw_21.inc"
};
static Sint16 draw_16_polygons[709] = {
#include "stg35/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[627] = {
#include "stg35/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg35/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg35/terrain/draw_20.inc"
};
static Sint16 draw_14_polygons[723] = {
#include "stg35/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[691] = {
#include "stg35/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg35/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg35/terrain/draw_15.inc"
};
static Sint16 draw_13_polygons[243] = {
#include "stg35/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[219] = {
#include "stg35/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg35/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg35/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[155] = {
#include "stg35/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[179] = {
#include "stg35/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg35/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg35/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[65] = {
#include "stg35/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[67] = {
#include "stg35/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg35/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg35/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[685] = {
#include "stg35/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[379] = {
#include "stg35/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg35/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg35/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[949] = {
#include "stg35/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[1155] = {
#include "stg35/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg35/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg35/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[545] = {
#include "stg35/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[475] = {
#include "stg35/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg35/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg35/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[147] = {
#include "stg35/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[111] = {
#include "stg35/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg35/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg35/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[1065] = {
#include "stg35/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[747] = {
#include "stg35/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg35/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg35/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[25] = {
#include "stg35/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[19] = {
#include "stg35/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg35/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg35/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[25] = {
#include "stg35/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[19] = {
#include "stg35/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg35/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg35/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[25] = {
#include "stg35/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[19] = {
#include "stg35/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg35/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg35/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[25] = {
#include "stg35/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[19] = {
#include "stg35/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg35/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg35/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[199] = {
#include "stg35/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[111] = {
#include "stg35/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg35/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg35/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[115] = {
#include "stg35/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[87] = {
#include "stg35/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg35/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg35/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg35/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[36] = {
#include "stg35/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg35/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[22] = {
#include "stg35/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg35/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg35/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg35/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[401] = {
#include "stg35/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg35/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[188] = {
#include "stg35/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg35/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg35/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg35/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[166] = {
#include "stg35/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg35/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[72] = {
#include "stg35/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg35/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg35/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg35/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[79] = {
#include "stg35/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg35/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[33] = {
#include "stg35/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg35/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg35/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg35/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[136] = {
#include "stg35/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg35/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[68] = {
#include "stg35/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg35/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg35/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg35/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[36] = {
#include "stg35/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg35/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[22] = {
#include "stg35/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg35/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg35/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg35/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[14] = {
#include "stg35/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg35/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[12] = {
#include "stg35/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg35/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg35/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg35/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[38] = {
#include "stg35/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg35/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[24] = {
#include "stg35/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg35/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg35/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg35/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[10] = {
#include "stg35/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg35/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[8] = {
#include "stg35/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg35/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg35/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg35/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[14] = {
#include "stg35/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg35/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[9] = {
#include "stg35/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg35/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg35/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg35/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[15] = {
#include "stg35/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg35/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[8] = {
#include "stg35/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg35/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg35/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg35/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[42] = {
#include "stg35/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg35/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[21] = {
#include "stg35/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg35/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg35/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg35/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[33] = {
#include "stg35/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg35/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[21] = {
#include "stg35/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg35/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg35/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg35/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[52] = {
#include "stg35/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg35/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[24] = {
#include "stg35/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg35/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg35/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg35/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[20] = {
#include "stg35/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg35/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[12] = {
#include "stg35/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg35/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg35/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg35/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[56] = {
#include "stg35/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg35/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[25] = {
#include "stg35/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg35/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg35/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg35/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[20] = {
#include "stg35/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg35/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[10] = {
#include "stg35/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg35/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg35/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg35/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[19] = {
#include "stg35/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg35/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[10] = {
#include "stg35/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg35/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg35/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg35/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[73] = {
#include "stg35/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg35/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[48] = {
#include "stg35/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg35/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg35/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg35/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[133] = {
#include "stg35/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg35/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[60] = {
#include "stg35/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg35/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg35/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg35/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[133] = {
#include "stg35/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg35/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[60] = {
#include "stg35/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg35/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg35/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg35/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[133] = {
#include "stg35/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg35/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[60] = {
#include "stg35/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg35/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg35/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg35/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[144] = {
#include "stg35/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg35/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[54] = {
#include "stg35/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg35/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg35/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_78_materials[1] = {
#include "stg35/terrain/collision_78_materials.inc"
};
static Sint16 collision_78_faces[16] = {
#include "stg35/terrain/collision_78_faces.inc"
};
static NJS_MESHSET collision_78_meshsets[1] = {
#include "stg35/terrain/collision_78_meshsets.inc"
};
static NJS_POINT3 collision_78_points[8] = {
#include "stg35/terrain/collision_78_points.inc"
};
static NJS_MODEL collision_78_model = {
#include "stg35/terrain/collision_78_model.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg35/terrain/collision_78.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg35/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[16] = {
#include "stg35/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg35/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[8] = {
#include "stg35/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg35/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg35/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_80_materials[1] = {
#include "stg35/terrain/collision_80_materials.inc"
};
static Sint16 collision_80_faces[16] = {
#include "stg35/terrain/collision_80_faces.inc"
};
static NJS_MESHSET collision_80_meshsets[1] = {
#include "stg35/terrain/collision_80_meshsets.inc"
};
static NJS_POINT3 collision_80_points[8] = {
#include "stg35/terrain/collision_80_points.inc"
};
static NJS_MODEL collision_80_model = {
#include "stg35/terrain/collision_80_model.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg35/terrain/collision_80.inc"
};
static NJS_MATERIAL collision_81_materials[1] = {
#include "stg35/terrain/collision_81_materials.inc"
};
static Sint16 collision_81_faces[16] = {
#include "stg35/terrain/collision_81_faces.inc"
};
static NJS_MESHSET collision_81_meshsets[1] = {
#include "stg35/terrain/collision_81_meshsets.inc"
};
static NJS_POINT3 collision_81_points[8] = {
#include "stg35/terrain/collision_81_points.inc"
};
static NJS_MODEL collision_81_model = {
#include "stg35/terrain/collision_81_model.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg35/terrain/collision_81.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg35/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[16] = {
#include "stg35/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg35/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[8] = {
#include "stg35/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg35/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg35/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg35/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[16] = {
#include "stg35/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg35/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[8] = {
#include "stg35/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg35/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg35/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg35/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[16] = {
#include "stg35/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg35/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[8] = {
#include "stg35/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg35/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg35/terrain/collision_84.inc"
};
static NJS_MATERIAL collision_85_materials[1] = {
#include "stg35/terrain/collision_85_materials.inc"
};
static Sint16 collision_85_faces[18] = {
#include "stg35/terrain/collision_85_faces.inc"
};
static NJS_MESHSET collision_85_meshsets[1] = {
#include "stg35/terrain/collision_85_meshsets.inc"
};
static NJS_POINT3 collision_85_points[12] = {
#include "stg35/terrain/collision_85_points.inc"
};
static NJS_MODEL collision_85_model = {
#include "stg35/terrain/collision_85_model.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg35/terrain/collision_85.inc"
};
static NJS_MATERIAL collision_86_materials[1] = {
#include "stg35/terrain/collision_86_materials.inc"
};
static Sint16 collision_86_faces[14] = {
#include "stg35/terrain/collision_86_faces.inc"
};
static NJS_MESHSET collision_86_meshsets[1] = {
#include "stg35/terrain/collision_86_meshsets.inc"
};
static NJS_POINT3 collision_86_points[9] = {
#include "stg35/terrain/collision_86_points.inc"
};
static NJS_MODEL collision_86_model = {
#include "stg35/terrain/collision_86_model.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg35/terrain/collision_86.inc"
};
static NJS_MATERIAL collision_87_materials[1] = {
#include "stg35/terrain/collision_87_materials.inc"
};
static Sint16 collision_87_faces[30] = {
#include "stg35/terrain/collision_87_faces.inc"
};
static NJS_MESHSET collision_87_meshsets[1] = {
#include "stg35/terrain/collision_87_meshsets.inc"
};
static NJS_POINT3 collision_87_points[21] = {
#include "stg35/terrain/collision_87_points.inc"
};
static NJS_MODEL collision_87_model = {
#include "stg35/terrain/collision_87_model.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg35/terrain/collision_87.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg35/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[18] = {
#include "stg35/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg35/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[12] = {
#include "stg35/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg35/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg35/terrain/collision_88.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg35/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg35/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg35/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg35/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg35/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg35/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg35/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg35/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg35/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg35/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg35/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg35/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg35/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg35/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg35/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg35/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg35/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg35/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg35/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg35/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg35/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg35/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg35/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg35/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg35/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg35/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg35/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg35/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg35/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg35/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg35/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg35/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg35/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg35/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg35/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg35/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg35/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg35/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg35/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg35/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg35/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg35/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg35/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg35/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg35/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg35/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg35/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg35/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg35/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg35/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg35/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg35/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg35/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg35/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg35/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg35/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg35/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg35/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg35/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg35/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg35/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg35/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg35/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg35/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg35/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg35/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg35/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg35/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg35/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg35/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg35/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg35/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg35/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg35/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg35/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg35/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg35/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg35/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg35/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg35/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg35/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg35/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg35/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg35/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg35/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg35/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg35/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg35/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg35/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg35/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg35/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg35/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg35/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg35/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg35/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg35/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg35/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg35/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg35/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg35/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg35/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg35/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg35/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg35/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg35/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg35/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg35/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg35/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg35/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg35/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg35/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg35/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg35/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg35/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg35/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg35/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg35/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg35/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg35/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg35/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg35/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg35/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg35/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg35/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg35/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg35/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg35/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg35/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg35/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg35/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg35/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg35/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg35/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg35/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg35/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg35/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg35/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg35/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg35/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg35/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg35/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg35/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg35/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg35/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg35/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg35/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg35/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg35/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg35/terrain/draw_14.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg35/terrain/collision_5.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg35/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg35/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg35/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg35/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg35/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg35/terrain/collision_13.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg35/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg35/terrain/collision_16.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg35/terrain/collision_17.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg35/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg35/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg35/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg35/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg35/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg35/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg35/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg35/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg35/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg35/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg35/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg35/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg35/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg35/terrain/collision_31.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg35/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg35/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg35/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg35/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg35/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg35/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg35/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg35/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg35/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg35/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg35/terrain/collision_43.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg35/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg35/terrain/collision_50.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg35/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg35/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg35/terrain/collision_54.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg35/terrain/collision_55.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg35/terrain/collision_56.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg35/terrain/collision_57.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg35/terrain/collision_59.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg35/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg35/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg35/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg35/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg35/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg35/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg35/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg35/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg35/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg35/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg35/terrain/collision_71.inc"
};
