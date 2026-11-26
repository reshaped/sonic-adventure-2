/* STG05.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between fire_statue.c and
 * object_archives.c, and its D lies between fire_statue.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg05/pumpkin_hill.h"

#include "stg05/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[159];
struct land_table pumpkin_hill_land;
static NJS_TEXNAME textures[24];
NJS_TEXLIST pumpkin_hill_land_texlist;
static Sint16 draw_73_polygons[1015];
static Sint32 draw_73_vertices[611];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_72_polygons[395];
static Sint32 draw_72_vertices[207];
static NJS_CNK_MODEL draw_72_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_70_polygons[103];
static Sint32 draw_70_vertices[75];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_71;
static Sint16 draw_69_polygons[97];
static Sint32 draw_69_vertices[75];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[95];
static Sint32 draw_68_vertices[107];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_66_polygons[97];
static Sint32 draw_66_vertices[75];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_65_polygons[409];
static Sint32 draw_65_vertices[203];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[327];
static Sint32 draw_64_vertices[211];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[383];
static Sint32 draw_63_vertices[195];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_61_polygons[61];
static Sint32 draw_61_vertices[51];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_60_polygons[877];
static Sint32 draw_60_vertices[555];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[863];
static Sint32 draw_59_vertices[827];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[557];
static Sint32 draw_58_vertices[351];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[61];
static Sint32 draw_57_vertices[51];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[353];
static Sint32 draw_56_vertices[207];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[259];
static Sint32 draw_55_vertices[171];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[793];
static Sint32 draw_54_vertices[443];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_52_polygons[807];
static Sint32 draw_52_vertices[427];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_51_polygons[509];
static Sint32 draw_51_vertices[263];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[165];
static Sint32 draw_50_vertices[127];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_49_polygons[185];
static Sint32 draw_49_vertices[123];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_48_polygons[599];
static Sint32 draw_48_vertices[575];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_46_polygons[61];
static Sint32 draw_46_vertices[51];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_44_polygons[895];
static Sint32 draw_44_vertices[495];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_43_polygons[1033];
static Sint32 draw_43_vertices[615];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_41_polygons[121];
static Sint32 draw_41_vertices[79];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_40_polygons[95];
static Sint32 draw_40_vertices[59];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[143];
static Sint32 draw_39_vertices[119];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[101];
static Sint32 draw_38_vertices[99];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[307];
static Sint32 draw_37_vertices[195];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[359];
static Sint32 draw_36_vertices[183];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_33_polygons[335];
static Sint32 draw_33_vertices[207];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_32_polygons[195];
static Sint32 draw_32_vertices[143];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[121];
static Sint32 draw_31_vertices[131];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[323];
static Sint32 draw_30_vertices[207];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[225];
static Sint32 draw_29_vertices[159];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[115];
static Sint32 draw_28_vertices[107];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[475];
static Sint32 draw_27_vertices[347];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[849];
static Sint32 draw_26_vertices[831];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[127];
static Sint32 draw_25_vertices[135];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[1049];
static Sint32 draw_24_vertices[1191];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[205];
static Sint32 draw_23_vertices[123];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[423];
static Sint32 draw_22_vertices[463];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[205];
static Sint32 draw_21_vertices[119];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[175];
static Sint32 draw_20_vertices[175];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[107];
static Sint32 draw_19_vertices[75];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[669];
static Sint32 draw_18_vertices[443];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_12_polygons[373];
static Sint32 draw_12_vertices[211];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_10_polygons[265];
static Sint32 draw_10_vertices[151];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_7_polygons[247];
static Sint32 draw_7_vertices[143];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_4_polygons[201];
static Sint32 draw_4_vertices[99];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_3_polygons[189];
static Sint32 draw_3_vertices[83];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[189];
static Sint32 draw_2_vertices[83];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[191];
static Sint32 draw_1_vertices[135];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[183];
static Sint32 draw_0_vertices[91];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[49];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[24];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[251];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[96];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[174];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[70];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[41];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[19];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[76];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[36];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[51];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[48];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[10];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[8];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[67];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[30];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[62];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[24];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[78];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[36];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[155];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[113];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[142];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[87];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[96];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[50];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[96];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[45];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[39];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[17];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[39];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[17];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[137];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[50];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[321];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[135];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[117];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[46];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[71];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[30];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[21];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[16];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[17];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[12];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[11];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[9];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[66];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[33];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[60];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[30];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[72];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[30];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[246];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[125];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[78];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[36];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[75];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[36];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[121];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[51];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[58];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[27];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[50];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[22];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_50_materials[1];
static Sint16 collision_50_faces[136];
static NJS_MESHSET collision_50_meshsets[1];
static NJS_POINT3 collision_50_points[114];
static NJS_MODEL collision_50_model;
static NJS_OBJECT collision_50;
static NJS_MATERIAL collision_51_materials[1];
static Sint16 collision_51_faces[38];
static NJS_MESHSET collision_51_meshsets[1];
static NJS_POINT3 collision_51_points[16];
static NJS_MODEL collision_51_model;
static NJS_OBJECT collision_51;
static NJS_MATERIAL collision_52_materials[1];
static Sint16 collision_52_faces[50];
static NJS_MESHSET collision_52_meshsets[1];
static NJS_POINT3 collision_52_points[48];
static NJS_MODEL collision_52_model;
static NJS_OBJECT collision_52;
static NJS_MATERIAL collision_53_materials[1];
static Sint16 collision_53_faces[48];
static NJS_MESHSET collision_53_meshsets[1];
static NJS_POINT3 collision_53_points[24];
static NJS_MODEL collision_53_model;
static NJS_OBJECT collision_53;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[18];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[16];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[147];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[116];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[37];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[16];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[28];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[16];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[80];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[60];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[228];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[155];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[52];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[28];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[162];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[104];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[330];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[208];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[122];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[72];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[54];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[32];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_65_materials[1];
static Sint16 collision_65_faces[22];
static NJS_MESHSET collision_65_meshsets[1];
static NJS_POINT3 collision_65_points[12];
static NJS_MODEL collision_65_model;
static NJS_OBJECT collision_65;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[26];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[18];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[22];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[15];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_79_materials[1];
static Sint16 collision_79_faces[18];
static NJS_MESHSET collision_79_meshsets[1];
static NJS_POINT3 collision_79_points[12];
static NJS_MODEL collision_79_model;
static NJS_OBJECT collision_79;
static NJS_MATERIAL collision_82_materials[1];
static Sint16 collision_82_faces[38];
static NJS_MESHSET collision_82_meshsets[1];
static NJS_POINT3 collision_82_points[27];
static NJS_MODEL collision_82_model;
static NJS_OBJECT collision_82;
static NJS_MATERIAL collision_83_materials[1];
static Sint16 collision_83_faces[147];
static NJS_MESHSET collision_83_meshsets[1];
static NJS_POINT3 collision_83_points[70];
static NJS_MODEL collision_83_model;
static NJS_OBJECT collision_83;
static NJS_MATERIAL collision_84_materials[1];
static Sint16 collision_84_faces[186];
static NJS_MESHSET collision_84_meshsets[1];
static NJS_POINT3 collision_84_points[133];
static NJS_MODEL collision_84_model;
static NJS_OBJECT collision_84;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_67;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_73;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;

static struct land_entry entries[159] = {
#include "stg05/terrain/entries.inc"
};
struct land_table pumpkin_hill_land = {
#include "stg05/terrain/pumpkin_hill_land.inc"
};
static NJS_TEXNAME textures[24] = {
#include "stg05/terrain/textures.inc"
};
NJS_TEXLIST pumpkin_hill_land_texlist = {
#include "stg05/terrain/pumpkin_hill_land_texlist.inc"
};
static Sint16 draw_73_polygons[1015] = {
#include "stg05/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[611] = {
#include "stg05/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg05/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg05/terrain/draw_73.inc"
};
static Sint16 draw_72_polygons[395] = {
#include "stg05/terrain/draw_72_polygons.inc"
};
static Sint32 draw_72_vertices[207] = {
#include "stg05/terrain/draw_72_vertices.inc"
};
static NJS_CNK_MODEL draw_72_model = {
#include "stg05/terrain/draw_72_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg05/terrain/draw_72.inc"
};
static Sint16 draw_70_polygons[103] = {
#include "stg05/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[75] = {
#include "stg05/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg05/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg05/terrain/draw_71.inc"
};
static Sint16 draw_69_polygons[97] = {
#include "stg05/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[75] = {
#include "stg05/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg05/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg05/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[95] = {
#include "stg05/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[107] = {
#include "stg05/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg05/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg05/terrain/draw_68.inc"
};
static Sint16 draw_66_polygons[97] = {
#include "stg05/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[75] = {
#include "stg05/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg05/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg05/terrain/draw_67.inc"
};
static Sint16 draw_65_polygons[409] = {
#include "stg05/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[203] = {
#include "stg05/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg05/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg05/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[327] = {
#include "stg05/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[211] = {
#include "stg05/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg05/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg05/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[383] = {
#include "stg05/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[195] = {
#include "stg05/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg05/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg05/terrain/draw_63.inc"
};
static Sint16 draw_61_polygons[61] = {
#include "stg05/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[51] = {
#include "stg05/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg05/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg05/terrain/draw_62.inc"
};
static Sint16 draw_60_polygons[877] = {
#include "stg05/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[555] = {
#include "stg05/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg05/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg05/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[863] = {
#include "stg05/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[827] = {
#include "stg05/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg05/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg05/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[557] = {
#include "stg05/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[351] = {
#include "stg05/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg05/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg05/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[61] = {
#include "stg05/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[51] = {
#include "stg05/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg05/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg05/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[353] = {
#include "stg05/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[207] = {
#include "stg05/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg05/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg05/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[259] = {
#include "stg05/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[171] = {
#include "stg05/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg05/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg05/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[793] = {
#include "stg05/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[443] = {
#include "stg05/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg05/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg05/terrain/draw_54.inc"
};
static Sint16 draw_52_polygons[807] = {
#include "stg05/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[427] = {
#include "stg05/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg05/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg05/terrain/draw_53.inc"
};
static Sint16 draw_51_polygons[509] = {
#include "stg05/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[263] = {
#include "stg05/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg05/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg05/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[165] = {
#include "stg05/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[127] = {
#include "stg05/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg05/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg05/terrain/draw_50.inc"
};
static Sint16 draw_49_polygons[185] = {
#include "stg05/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[123] = {
#include "stg05/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg05/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg05/terrain/draw_49.inc"
};
static Sint16 draw_48_polygons[599] = {
#include "stg05/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[575] = {
#include "stg05/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg05/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg05/terrain/draw_48.inc"
};
static Sint16 draw_46_polygons[61] = {
#include "stg05/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[51] = {
#include "stg05/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg05/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg05/terrain/draw_47.inc"
};
static Sint16 draw_44_polygons[895] = {
#include "stg05/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[495] = {
#include "stg05/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg05/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg05/terrain/draw_45.inc"
};
static Sint16 draw_43_polygons[1033] = {
#include "stg05/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[615] = {
#include "stg05/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg05/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg05/terrain/draw_43.inc"
};
static Sint16 draw_41_polygons[121] = {
#include "stg05/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[79] = {
#include "stg05/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg05/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg05/terrain/draw_42.inc"
};
static Sint16 draw_40_polygons[95] = {
#include "stg05/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[59] = {
#include "stg05/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg05/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg05/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[143] = {
#include "stg05/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[119] = {
#include "stg05/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg05/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg05/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[101] = {
#include "stg05/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[99] = {
#include "stg05/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg05/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg05/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[307] = {
#include "stg05/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[195] = {
#include "stg05/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg05/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg05/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[359] = {
#include "stg05/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[183] = {
#include "stg05/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg05/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg05/terrain/draw_36.inc"
};
static Sint16 draw_33_polygons[335] = {
#include "stg05/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[207] = {
#include "stg05/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg05/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg05/terrain/draw_35.inc"
};
static Sint16 draw_32_polygons[195] = {
#include "stg05/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[143] = {
#include "stg05/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg05/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg05/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[121] = {
#include "stg05/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[131] = {
#include "stg05/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg05/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg05/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[323] = {
#include "stg05/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[207] = {
#include "stg05/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg05/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg05/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[225] = {
#include "stg05/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[159] = {
#include "stg05/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg05/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg05/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[115] = {
#include "stg05/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[107] = {
#include "stg05/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg05/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg05/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[475] = {
#include "stg05/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[347] = {
#include "stg05/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg05/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg05/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[849] = {
#include "stg05/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[831] = {
#include "stg05/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg05/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg05/terrain/draw_26.inc"
};
static Sint16 draw_25_polygons[127] = {
#include "stg05/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[135] = {
#include "stg05/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg05/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg05/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[1049] = {
#include "stg05/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[1191] = {
#include "stg05/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg05/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg05/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[205] = {
#include "stg05/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[123] = {
#include "stg05/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg05/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg05/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[423] = {
#include "stg05/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[463] = {
#include "stg05/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg05/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg05/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[205] = {
#include "stg05/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[119] = {
#include "stg05/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg05/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg05/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[175] = {
#include "stg05/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[175] = {
#include "stg05/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg05/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg05/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[107] = {
#include "stg05/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[75] = {
#include "stg05/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg05/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg05/terrain/draw_19.inc"
};
static Sint16 draw_18_polygons[669] = {
#include "stg05/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[443] = {
#include "stg05/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg05/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg05/terrain/draw_18.inc"
};
static Sint16 draw_12_polygons[373] = {
#include "stg05/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[211] = {
#include "stg05/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg05/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg05/terrain/draw_17.inc"
};
static Sint16 draw_10_polygons[265] = {
#include "stg05/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[151] = {
#include "stg05/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg05/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg05/terrain/draw_11.inc"
};
static Sint16 draw_7_polygons[247] = {
#include "stg05/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[143] = {
#include "stg05/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg05/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg05/terrain/draw_9.inc"
};
static Sint16 draw_4_polygons[201] = {
#include "stg05/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[99] = {
#include "stg05/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg05/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg05/terrain/draw_6.inc"
};
static Sint16 draw_3_polygons[189] = {
#include "stg05/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[83] = {
#include "stg05/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg05/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg05/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[189] = {
#include "stg05/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[83] = {
#include "stg05/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg05/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg05/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[191] = {
#include "stg05/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[135] = {
#include "stg05/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg05/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg05/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[183] = {
#include "stg05/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[91] = {
#include "stg05/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg05/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg05/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg05/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[49] = {
#include "stg05/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg05/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[24] = {
#include "stg05/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg05/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg05/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg05/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[251] = {
#include "stg05/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg05/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[96] = {
#include "stg05/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg05/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg05/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg05/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[174] = {
#include "stg05/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg05/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[70] = {
#include "stg05/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg05/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg05/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg05/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[41] = {
#include "stg05/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg05/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[19] = {
#include "stg05/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg05/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg05/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg05/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[76] = {
#include "stg05/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg05/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[36] = {
#include "stg05/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg05/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg05/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg05/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[51] = {
#include "stg05/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg05/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[48] = {
#include "stg05/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg05/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg05/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg05/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[10] = {
#include "stg05/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg05/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[8] = {
#include "stg05/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg05/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg05/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg05/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[67] = {
#include "stg05/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg05/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[30] = {
#include "stg05/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg05/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg05/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg05/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[62] = {
#include "stg05/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg05/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[24] = {
#include "stg05/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg05/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg05/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg05/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[78] = {
#include "stg05/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg05/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[36] = {
#include "stg05/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg05/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg05/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg05/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[155] = {
#include "stg05/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg05/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[113] = {
#include "stg05/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg05/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg05/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg05/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[142] = {
#include "stg05/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg05/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[87] = {
#include "stg05/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg05/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg05/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg05/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[96] = {
#include "stg05/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg05/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[50] = {
#include "stg05/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg05/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg05/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg05/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[96] = {
#include "stg05/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg05/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[45] = {
#include "stg05/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg05/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg05/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg05/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[39] = {
#include "stg05/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg05/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[17] = {
#include "stg05/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg05/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg05/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg05/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[39] = {
#include "stg05/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg05/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[17] = {
#include "stg05/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg05/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg05/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg05/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[137] = {
#include "stg05/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg05/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[50] = {
#include "stg05/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg05/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg05/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg05/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[321] = {
#include "stg05/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg05/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[135] = {
#include "stg05/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg05/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg05/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg05/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[117] = {
#include "stg05/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg05/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[46] = {
#include "stg05/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg05/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg05/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg05/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[71] = {
#include "stg05/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg05/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[30] = {
#include "stg05/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg05/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg05/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg05/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[21] = {
#include "stg05/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg05/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[16] = {
#include "stg05/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg05/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg05/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg05/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[17] = {
#include "stg05/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg05/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[12] = {
#include "stg05/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg05/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg05/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg05/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[11] = {
#include "stg05/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg05/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[9] = {
#include "stg05/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg05/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg05/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg05/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[66] = {
#include "stg05/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg05/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[33] = {
#include "stg05/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg05/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg05/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg05/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[60] = {
#include "stg05/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg05/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[30] = {
#include "stg05/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg05/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg05/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg05/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[72] = {
#include "stg05/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg05/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[30] = {
#include "stg05/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg05/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg05/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg05/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[246] = {
#include "stg05/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg05/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[125] = {
#include "stg05/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg05/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg05/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg05/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[78] = {
#include "stg05/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg05/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[36] = {
#include "stg05/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg05/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg05/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg05/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[75] = {
#include "stg05/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg05/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[36] = {
#include "stg05/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg05/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg05/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg05/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[121] = {
#include "stg05/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg05/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[51] = {
#include "stg05/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg05/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg05/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg05/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[58] = {
#include "stg05/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg05/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[27] = {
#include "stg05/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg05/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg05/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg05/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[50] = {
#include "stg05/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg05/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[22] = {
#include "stg05/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg05/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg05/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_50_materials[1] = {
#include "stg05/terrain/collision_50_materials.inc"
};
static Sint16 collision_50_faces[136] = {
#include "stg05/terrain/collision_50_faces.inc"
};
static NJS_MESHSET collision_50_meshsets[1] = {
#include "stg05/terrain/collision_50_meshsets.inc"
};
static NJS_POINT3 collision_50_points[114] = {
#include "stg05/terrain/collision_50_points.inc"
};
static NJS_MODEL collision_50_model = {
#include "stg05/terrain/collision_50_model.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg05/terrain/collision_50.inc"
};
static NJS_MATERIAL collision_51_materials[1] = {
#include "stg05/terrain/collision_51_materials.inc"
};
static Sint16 collision_51_faces[38] = {
#include "stg05/terrain/collision_51_faces.inc"
};
static NJS_MESHSET collision_51_meshsets[1] = {
#include "stg05/terrain/collision_51_meshsets.inc"
};
static NJS_POINT3 collision_51_points[16] = {
#include "stg05/terrain/collision_51_points.inc"
};
static NJS_MODEL collision_51_model = {
#include "stg05/terrain/collision_51_model.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg05/terrain/collision_51.inc"
};
static NJS_MATERIAL collision_52_materials[1] = {
#include "stg05/terrain/collision_52_materials.inc"
};
static Sint16 collision_52_faces[50] = {
#include "stg05/terrain/collision_52_faces.inc"
};
static NJS_MESHSET collision_52_meshsets[1] = {
#include "stg05/terrain/collision_52_meshsets.inc"
};
static NJS_POINT3 collision_52_points[48] = {
#include "stg05/terrain/collision_52_points.inc"
};
static NJS_MODEL collision_52_model = {
#include "stg05/terrain/collision_52_model.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg05/terrain/collision_52.inc"
};
static NJS_MATERIAL collision_53_materials[1] = {
#include "stg05/terrain/collision_53_materials.inc"
};
static Sint16 collision_53_faces[48] = {
#include "stg05/terrain/collision_53_faces.inc"
};
static NJS_MESHSET collision_53_meshsets[1] = {
#include "stg05/terrain/collision_53_meshsets.inc"
};
static NJS_POINT3 collision_53_points[24] = {
#include "stg05/terrain/collision_53_points.inc"
};
static NJS_MODEL collision_53_model = {
#include "stg05/terrain/collision_53_model.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg05/terrain/collision_53.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg05/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[18] = {
#include "stg05/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg05/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[16] = {
#include "stg05/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg05/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg05/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg05/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[147] = {
#include "stg05/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg05/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[116] = {
#include "stg05/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg05/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg05/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg05/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[37] = {
#include "stg05/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg05/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[16] = {
#include "stg05/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg05/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg05/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg05/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[28] = {
#include "stg05/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg05/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[16] = {
#include "stg05/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg05/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg05/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg05/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[80] = {
#include "stg05/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg05/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[60] = {
#include "stg05/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg05/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg05/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg05/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[228] = {
#include "stg05/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg05/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[155] = {
#include "stg05/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg05/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg05/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg05/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[52] = {
#include "stg05/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg05/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[28] = {
#include "stg05/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg05/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg05/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg05/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[162] = {
#include "stg05/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg05/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[104] = {
#include "stg05/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg05/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg05/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg05/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[330] = {
#include "stg05/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg05/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[208] = {
#include "stg05/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg05/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg05/terrain/collision_62.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg05/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[122] = {
#include "stg05/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg05/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[72] = {
#include "stg05/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg05/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg05/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg05/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[54] = {
#include "stg05/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg05/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[32] = {
#include "stg05/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg05/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg05/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_65_materials[1] = {
#include "stg05/terrain/collision_65_materials.inc"
};
static Sint16 collision_65_faces[22] = {
#include "stg05/terrain/collision_65_faces.inc"
};
static NJS_MESHSET collision_65_meshsets[1] = {
#include "stg05/terrain/collision_65_meshsets.inc"
};
static NJS_POINT3 collision_65_points[12] = {
#include "stg05/terrain/collision_65_points.inc"
};
static NJS_MODEL collision_65_model = {
#include "stg05/terrain/collision_65_model.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg05/terrain/collision_65.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg05/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[26] = {
#include "stg05/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg05/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[18] = {
#include "stg05/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg05/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg05/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg05/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[22] = {
#include "stg05/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg05/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[15] = {
#include "stg05/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg05/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg05/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_79_materials[1] = {
#include "stg05/terrain/collision_79_materials.inc"
};
static Sint16 collision_79_faces[18] = {
#include "stg05/terrain/collision_79_faces.inc"
};
static NJS_MESHSET collision_79_meshsets[1] = {
#include "stg05/terrain/collision_79_meshsets.inc"
};
static NJS_POINT3 collision_79_points[12] = {
#include "stg05/terrain/collision_79_points.inc"
};
static NJS_MODEL collision_79_model = {
#include "stg05/terrain/collision_79_model.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg05/terrain/collision_79.inc"
};
static NJS_MATERIAL collision_82_materials[1] = {
#include "stg05/terrain/collision_82_materials.inc"
};
static Sint16 collision_82_faces[38] = {
#include "stg05/terrain/collision_82_faces.inc"
};
static NJS_MESHSET collision_82_meshsets[1] = {
#include "stg05/terrain/collision_82_meshsets.inc"
};
static NJS_POINT3 collision_82_points[27] = {
#include "stg05/terrain/collision_82_points.inc"
};
static NJS_MODEL collision_82_model = {
#include "stg05/terrain/collision_82_model.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg05/terrain/collision_82.inc"
};
static NJS_MATERIAL collision_83_materials[1] = {
#include "stg05/terrain/collision_83_materials.inc"
};
static Sint16 collision_83_faces[147] = {
#include "stg05/terrain/collision_83_faces.inc"
};
static NJS_MESHSET collision_83_meshsets[1] = {
#include "stg05/terrain/collision_83_meshsets.inc"
};
static NJS_POINT3 collision_83_points[70] = {
#include "stg05/terrain/collision_83_points.inc"
};
static NJS_MODEL collision_83_model = {
#include "stg05/terrain/collision_83_model.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg05/terrain/collision_83.inc"
};
static NJS_MATERIAL collision_84_materials[1] = {
#include "stg05/terrain/collision_84_materials.inc"
};
static Sint16 collision_84_faces[186] = {
#include "stg05/terrain/collision_84_faces.inc"
};
static NJS_MESHSET collision_84_meshsets[1] = {
#include "stg05/terrain/collision_84_meshsets.inc"
};
static NJS_POINT3 collision_84_points[133] = {
#include "stg05/terrain/collision_84_points.inc"
};
static NJS_MODEL collision_84_model = {
#include "stg05/terrain/collision_84_model.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg05/terrain/collision_84.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg05/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg05/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg05/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg05/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg05/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg05/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg05/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg05/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg05/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg05/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg05/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg05/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg05/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg05/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg05/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg05/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg05/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg05/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg05/terrain/draw_4.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg05/terrain/collision_1.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg05/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg05/terrain/collision_11.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg05/terrain/collision_13.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg05/terrain/collision_23.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg05/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg05/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg05/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg05/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg05/terrain/collision_29.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg05/terrain/collision_31.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg05/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg05/terrain/collision_34.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg05/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg05/terrain/collision_37.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg05/terrain/collision_41.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg05/terrain/collision_45.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg05/terrain/collision_47.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg05/terrain/collision_66.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg05/terrain/collision_67.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg05/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg05/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg05/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg05/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg05/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg05/terrain/collision_73.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg05/terrain/collision_75.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg05/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg05/terrain/collision_78.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg05/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg05/terrain/collision_81.inc"
};
