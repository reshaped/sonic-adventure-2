#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg07/aquatic_mine.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char aquatic_mine_land_filenames[229] = {
#include "stg07/terrain/aquatic_mine_land_filenames.inc"
};

/* STG07.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between aquatic_mine.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[116];
struct land_table aquatic_mine_land;
static NJS_TEXNAME textures[19];
NJS_TEXLIST aquatic_mine_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[30];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[24];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[70];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[24];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[87];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[38];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[174];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[68];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[45];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[24];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[81];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[41];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[63];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[29];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[54];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[28];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[47];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[28];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[37];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[18];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[45];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[20];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[42];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[18];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[54];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[30];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[44];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[23];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[58];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[25];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[75];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[28];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[72];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[25];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[57];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[28];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[100];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[45];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[94];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[36];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[53];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[22];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[11];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[8];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[38];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[16];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[84];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[36];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[30];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[18];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[76];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[32];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[96];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[42];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[40];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[18];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[37];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[24];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[55];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[40];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[19];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[8];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[53];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[24];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[29];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[15];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[29];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[12];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[18];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[10];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_35_materials[1];
static Sint16 collision_35_faces[27];
static NJS_MESHSET collision_35_meshsets[1];
static NJS_POINT3 collision_35_points[16];
static NJS_MODEL collision_35_model;
static NJS_OBJECT collision_35;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[26];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[13];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_37_materials[1];
static Sint16 collision_37_faces[55];
static NJS_MESHSET collision_37_meshsets[1];
static NJS_POINT3 collision_37_points[40];
static NJS_MODEL collision_37_model;
static NJS_OBJECT collision_37;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[36];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[16];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[15];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[8];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[50];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[36];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[48];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[30];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[84];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[54];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_43_materials[1];
static Sint16 collision_43_faces[33];
static NJS_MESHSET collision_43_meshsets[1];
static NJS_POINT3 collision_43_points[16];
static NJS_MODEL collision_43_model;
static NJS_OBJECT collision_43;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[27];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[16];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_45_materials[1];
static Sint16 collision_45_faces[49];
static NJS_MESHSET collision_45_meshsets[1];
static NJS_POINT3 collision_45_points[24];
static NJS_MODEL collision_45_model;
static NJS_OBJECT collision_45;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[11];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[8];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[33];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[16];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[22];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[12];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[19];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[16];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[25];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[15];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[57];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[22];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[33];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[16];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[7];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[6];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[9];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[8];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_61_materials[1];
static Sint16 collision_61_faces[9];
static NJS_MESHSET collision_61_meshsets[1];
static NJS_POINT3 collision_61_points[8];
static NJS_MODEL collision_61_model;
static NJS_OBJECT collision_61;
static NJS_MATERIAL collision_62_materials[1];
static Sint16 collision_62_faces[11];
static NJS_MESHSET collision_62_meshsets[1];
static NJS_POINT3 collision_62_points[8];
static NJS_MODEL collision_62_model;
static NJS_OBJECT collision_62;
static Sint16 draw_52_polygons[185];
static Sint32 draw_52_vertices[99];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[97];
static Sint32 draw_51_vertices[51];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[277];
static Sint32 draw_50_vertices[115];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_49_polygons[787];
static Sint32 draw_49_vertices[279];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_48_polygons[201];
static Sint32 draw_48_vertices[99];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_47_polygons[127];
static Sint32 draw_47_vertices[107];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_46_polygons[63];
static Sint32 draw_46_vertices[67];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_45_polygons[119];
static Sint32 draw_45_vertices[83];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_44_polygons[181];
static Sint32 draw_44_vertices[99];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[101];
static Sint32 draw_43_vertices[51];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[277];
static Sint32 draw_42_vertices[251];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[109];
static Sint32 draw_41_vertices[83];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[195];
static Sint32 draw_40_vertices[115];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[315];
static Sint32 draw_39_vertices[275];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[63];
static Sint32 draw_38_vertices[67];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[63];
static Sint32 draw_37_vertices[51];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_34_polygons[189];
static Sint32 draw_34_vertices[175];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_33_polygons[401];
static Sint32 draw_33_vertices[155];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[229];
static Sint32 draw_32_vertices[127];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[131];
static Sint32 draw_31_vertices[91];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[117];
static Sint32 draw_30_vertices[51];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[49];
static Sint32 draw_29_vertices[47];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[157];
static Sint32 draw_28_vertices[115];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[305];
static Sint32 draw_27_vertices[131];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_25_polygons[299];
static Sint32 draw_25_vertices[195];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_24_polygons[95];
static Sint32 draw_24_vertices[99];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[219];
static Sint32 draw_23_vertices[87];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[355];
static Sint32 draw_22_vertices[175];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[89];
static Sint32 draw_21_vertices[63];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_18_polygons[73];
static Sint32 draw_18_vertices[67];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_16_polygons[63];
static Sint32 draw_16_vertices[51];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_15_polygons[143];
static Sint32 draw_15_vertices[71];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_13_polygons[175];
static Sint32 draw_13_vertices[99];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_12_polygons[275];
static Sint32 draw_12_vertices[111];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_11_polygons[491];
static Sint32 draw_11_vertices[167];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[229];
static Sint32 draw_10_vertices[95];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[191];
static Sint32 draw_9_vertices[83];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[271];
static Sint32 draw_8_vertices[123];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_4_polygons[63];
static Sint32 draw_4_vertices[59];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_3_polygons[273];
static Sint32 draw_3_vertices[147];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[141];
static Sint32 draw_2_vertices[75];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[363];
static Sint32 draw_1_vertices[195];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[141];
static Sint32 draw_0_vertices[75];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_57;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;

static struct land_entry entries[116] = {
#include "stg07/terrain/entries.inc"
};
struct land_table aquatic_mine_land = {
#include "stg07/terrain/aquatic_mine_land.inc"
};
static NJS_TEXNAME textures[19] = {
#include "stg07/terrain/textures.inc"
};
NJS_TEXLIST aquatic_mine_land_texlist = {
#include "stg07/terrain/aquatic_mine_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg07/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[30] = {
#include "stg07/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg07/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[24] = {
#include "stg07/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg07/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg07/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg07/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[70] = {
#include "stg07/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg07/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[24] = {
#include "stg07/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg07/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg07/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg07/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[87] = {
#include "stg07/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg07/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[38] = {
#include "stg07/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg07/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg07/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg07/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[174] = {
#include "stg07/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg07/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[68] = {
#include "stg07/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg07/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg07/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg07/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[45] = {
#include "stg07/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg07/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[24] = {
#include "stg07/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg07/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg07/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg07/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[81] = {
#include "stg07/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg07/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[41] = {
#include "stg07/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg07/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg07/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg07/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[63] = {
#include "stg07/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg07/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[29] = {
#include "stg07/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg07/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg07/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg07/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[54] = {
#include "stg07/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg07/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[28] = {
#include "stg07/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg07/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg07/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg07/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[47] = {
#include "stg07/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg07/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[28] = {
#include "stg07/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg07/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg07/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg07/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[37] = {
#include "stg07/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg07/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[18] = {
#include "stg07/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg07/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg07/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg07/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[45] = {
#include "stg07/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg07/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[20] = {
#include "stg07/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg07/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg07/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg07/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[42] = {
#include "stg07/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg07/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[18] = {
#include "stg07/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg07/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg07/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg07/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[54] = {
#include "stg07/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg07/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[30] = {
#include "stg07/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg07/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg07/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg07/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[44] = {
#include "stg07/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg07/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[23] = {
#include "stg07/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg07/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg07/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg07/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[58] = {
#include "stg07/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg07/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[25] = {
#include "stg07/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg07/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg07/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg07/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[75] = {
#include "stg07/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg07/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[28] = {
#include "stg07/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg07/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg07/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg07/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[72] = {
#include "stg07/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg07/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[25] = {
#include "stg07/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg07/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg07/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg07/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[57] = {
#include "stg07/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg07/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[28] = {
#include "stg07/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg07/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg07/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg07/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[100] = {
#include "stg07/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg07/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[45] = {
#include "stg07/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg07/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg07/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg07/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[94] = {
#include "stg07/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg07/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[36] = {
#include "stg07/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg07/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg07/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg07/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[53] = {
#include "stg07/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg07/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[22] = {
#include "stg07/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg07/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg07/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg07/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[11] = {
#include "stg07/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg07/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[8] = {
#include "stg07/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg07/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg07/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg07/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[38] = {
#include "stg07/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg07/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[16] = {
#include "stg07/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg07/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg07/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg07/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[84] = {
#include "stg07/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg07/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[36] = {
#include "stg07/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg07/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg07/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg07/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[30] = {
#include "stg07/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg07/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[18] = {
#include "stg07/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg07/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg07/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg07/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[76] = {
#include "stg07/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg07/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[32] = {
#include "stg07/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg07/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg07/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg07/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[96] = {
#include "stg07/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg07/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[42] = {
#include "stg07/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg07/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg07/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg07/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[40] = {
#include "stg07/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg07/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[18] = {
#include "stg07/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg07/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg07/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg07/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[37] = {
#include "stg07/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg07/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[24] = {
#include "stg07/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg07/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg07/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg07/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[55] = {
#include "stg07/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg07/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[40] = {
#include "stg07/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg07/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg07/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg07/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[19] = {
#include "stg07/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg07/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[8] = {
#include "stg07/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg07/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg07/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg07/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[53] = {
#include "stg07/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg07/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[24] = {
#include "stg07/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg07/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg07/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg07/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[29] = {
#include "stg07/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg07/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[15] = {
#include "stg07/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg07/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg07/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg07/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[29] = {
#include "stg07/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg07/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[12] = {
#include "stg07/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg07/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg07/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg07/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[18] = {
#include "stg07/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg07/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[10] = {
#include "stg07/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg07/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg07/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_35_materials[1] = {
#include "stg07/terrain/collision_35_materials.inc"
};
static Sint16 collision_35_faces[27] = {
#include "stg07/terrain/collision_35_faces.inc"
};
static NJS_MESHSET collision_35_meshsets[1] = {
#include "stg07/terrain/collision_35_meshsets.inc"
};
static NJS_POINT3 collision_35_points[16] = {
#include "stg07/terrain/collision_35_points.inc"
};
static NJS_MODEL collision_35_model = {
#include "stg07/terrain/collision_35_model.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg07/terrain/collision_35.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg07/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[26] = {
#include "stg07/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg07/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[13] = {
#include "stg07/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg07/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg07/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_37_materials[1] = {
#include "stg07/terrain/collision_37_materials.inc"
};
static Sint16 collision_37_faces[55] = {
#include "stg07/terrain/collision_37_faces.inc"
};
static NJS_MESHSET collision_37_meshsets[1] = {
#include "stg07/terrain/collision_37_meshsets.inc"
};
static NJS_POINT3 collision_37_points[40] = {
#include "stg07/terrain/collision_37_points.inc"
};
static NJS_MODEL collision_37_model = {
#include "stg07/terrain/collision_37_model.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg07/terrain/collision_37.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg07/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[36] = {
#include "stg07/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg07/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[16] = {
#include "stg07/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg07/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg07/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg07/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[15] = {
#include "stg07/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg07/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[8] = {
#include "stg07/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg07/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg07/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg07/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[50] = {
#include "stg07/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg07/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[36] = {
#include "stg07/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg07/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg07/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg07/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[48] = {
#include "stg07/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg07/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[30] = {
#include "stg07/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg07/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg07/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg07/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[84] = {
#include "stg07/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg07/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[54] = {
#include "stg07/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg07/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg07/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_43_materials[1] = {
#include "stg07/terrain/collision_43_materials.inc"
};
static Sint16 collision_43_faces[33] = {
#include "stg07/terrain/collision_43_faces.inc"
};
static NJS_MESHSET collision_43_meshsets[1] = {
#include "stg07/terrain/collision_43_meshsets.inc"
};
static NJS_POINT3 collision_43_points[16] = {
#include "stg07/terrain/collision_43_points.inc"
};
static NJS_MODEL collision_43_model = {
#include "stg07/terrain/collision_43_model.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg07/terrain/collision_43.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg07/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[27] = {
#include "stg07/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg07/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[16] = {
#include "stg07/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg07/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg07/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_45_materials[1] = {
#include "stg07/terrain/collision_45_materials.inc"
};
static Sint16 collision_45_faces[49] = {
#include "stg07/terrain/collision_45_faces.inc"
};
static NJS_MESHSET collision_45_meshsets[1] = {
#include "stg07/terrain/collision_45_meshsets.inc"
};
static NJS_POINT3 collision_45_points[24] = {
#include "stg07/terrain/collision_45_points.inc"
};
static NJS_MODEL collision_45_model = {
#include "stg07/terrain/collision_45_model.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg07/terrain/collision_45.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg07/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[11] = {
#include "stg07/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg07/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[8] = {
#include "stg07/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg07/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg07/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg07/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[33] = {
#include "stg07/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg07/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[16] = {
#include "stg07/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg07/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg07/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg07/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[22] = {
#include "stg07/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg07/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[12] = {
#include "stg07/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg07/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg07/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg07/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[19] = {
#include "stg07/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg07/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[16] = {
#include "stg07/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg07/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg07/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg07/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[25] = {
#include "stg07/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg07/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[15] = {
#include "stg07/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg07/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg07/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg07/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[57] = {
#include "stg07/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg07/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[22] = {
#include "stg07/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg07/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg07/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg07/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[33] = {
#include "stg07/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg07/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[16] = {
#include "stg07/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg07/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg07/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg07/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[7] = {
#include "stg07/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg07/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[6] = {
#include "stg07/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg07/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg07/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg07/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[9] = {
#include "stg07/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg07/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[8] = {
#include "stg07/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg07/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg07/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_61_materials[1] = {
#include "stg07/terrain/collision_61_materials.inc"
};
static Sint16 collision_61_faces[9] = {
#include "stg07/terrain/collision_61_faces.inc"
};
static NJS_MESHSET collision_61_meshsets[1] = {
#include "stg07/terrain/collision_61_meshsets.inc"
};
static NJS_POINT3 collision_61_points[8] = {
#include "stg07/terrain/collision_61_points.inc"
};
static NJS_MODEL collision_61_model = {
#include "stg07/terrain/collision_61_model.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg07/terrain/collision_61.inc"
};
static NJS_MATERIAL collision_62_materials[1] = {
#include "stg07/terrain/collision_62_materials.inc"
};
static Sint16 collision_62_faces[11] = {
#include "stg07/terrain/collision_62_faces.inc"
};
static NJS_MESHSET collision_62_meshsets[1] = {
#include "stg07/terrain/collision_62_meshsets.inc"
};
static NJS_POINT3 collision_62_points[8] = {
#include "stg07/terrain/collision_62_points.inc"
};
static NJS_MODEL collision_62_model = {
#include "stg07/terrain/collision_62_model.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg07/terrain/collision_62.inc"
};
static Sint16 draw_52_polygons[185] = {
#include "stg07/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[99] = {
#include "stg07/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg07/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg07/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[97] = {
#include "stg07/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[51] = {
#include "stg07/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg07/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg07/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[277] = {
#include "stg07/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[115] = {
#include "stg07/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg07/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg07/terrain/draw_50.inc"
};
static Sint16 draw_49_polygons[787] = {
#include "stg07/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[279] = {
#include "stg07/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg07/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg07/terrain/draw_49.inc"
};
static Sint16 draw_48_polygons[201] = {
#include "stg07/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[99] = {
#include "stg07/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg07/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg07/terrain/draw_48.inc"
};
static Sint16 draw_47_polygons[127] = {
#include "stg07/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[107] = {
#include "stg07/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg07/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg07/terrain/draw_47.inc"
};
static Sint16 draw_46_polygons[63] = {
#include "stg07/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[67] = {
#include "stg07/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg07/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg07/terrain/draw_46.inc"
};
static Sint16 draw_45_polygons[119] = {
#include "stg07/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[83] = {
#include "stg07/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg07/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg07/terrain/draw_45.inc"
};
static Sint16 draw_44_polygons[181] = {
#include "stg07/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[99] = {
#include "stg07/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg07/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg07/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[101] = {
#include "stg07/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[51] = {
#include "stg07/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg07/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg07/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[277] = {
#include "stg07/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[251] = {
#include "stg07/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg07/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg07/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[109] = {
#include "stg07/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[83] = {
#include "stg07/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg07/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg07/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[195] = {
#include "stg07/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[115] = {
#include "stg07/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg07/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg07/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[315] = {
#include "stg07/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[275] = {
#include "stg07/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg07/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg07/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[63] = {
#include "stg07/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[67] = {
#include "stg07/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg07/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg07/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[63] = {
#include "stg07/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[51] = {
#include "stg07/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg07/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg07/terrain/draw_37.inc"
};
static Sint16 draw_34_polygons[189] = {
#include "stg07/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[175] = {
#include "stg07/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg07/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg07/terrain/draw_36.inc"
};
static Sint16 draw_33_polygons[401] = {
#include "stg07/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[155] = {
#include "stg07/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg07/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg07/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[229] = {
#include "stg07/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[127] = {
#include "stg07/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg07/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg07/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[131] = {
#include "stg07/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[91] = {
#include "stg07/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg07/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg07/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[117] = {
#include "stg07/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[51] = {
#include "stg07/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg07/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg07/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[49] = {
#include "stg07/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[47] = {
#include "stg07/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg07/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg07/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[157] = {
#include "stg07/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[115] = {
#include "stg07/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg07/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg07/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[305] = {
#include "stg07/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[131] = {
#include "stg07/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg07/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg07/terrain/draw_27.inc"
};
static Sint16 draw_25_polygons[299] = {
#include "stg07/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[195] = {
#include "stg07/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg07/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg07/terrain/draw_26.inc"
};
static Sint16 draw_24_polygons[95] = {
#include "stg07/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[99] = {
#include "stg07/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg07/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg07/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[219] = {
#include "stg07/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[87] = {
#include "stg07/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg07/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg07/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[355] = {
#include "stg07/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[175] = {
#include "stg07/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg07/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg07/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[89] = {
#include "stg07/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[63] = {
#include "stg07/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg07/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg07/terrain/draw_21.inc"
};
static Sint16 draw_18_polygons[73] = {
#include "stg07/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[67] = {
#include "stg07/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg07/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg07/terrain/draw_20.inc"
};
static Sint16 draw_16_polygons[63] = {
#include "stg07/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[51] = {
#include "stg07/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg07/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg07/terrain/draw_17.inc"
};
static Sint16 draw_15_polygons[143] = {
#include "stg07/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[71] = {
#include "stg07/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg07/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg07/terrain/draw_15.inc"
};
static Sint16 draw_13_polygons[175] = {
#include "stg07/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[99] = {
#include "stg07/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg07/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg07/terrain/draw_14.inc"
};
static Sint16 draw_12_polygons[275] = {
#include "stg07/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[111] = {
#include "stg07/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg07/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg07/terrain/draw_12.inc"
};
static Sint16 draw_11_polygons[491] = {
#include "stg07/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[167] = {
#include "stg07/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg07/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg07/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[229] = {
#include "stg07/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[95] = {
#include "stg07/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg07/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg07/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[191] = {
#include "stg07/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[83] = {
#include "stg07/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg07/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg07/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[271] = {
#include "stg07/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[123] = {
#include "stg07/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg07/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg07/terrain/draw_8.inc"
};
static Sint16 draw_4_polygons[63] = {
#include "stg07/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[59] = {
#include "stg07/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg07/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg07/terrain/draw_7.inc"
};
static Sint16 draw_3_polygons[273] = {
#include "stg07/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[147] = {
#include "stg07/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg07/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg07/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[141] = {
#include "stg07/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[75] = {
#include "stg07/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg07/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg07/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[363] = {
#include "stg07/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[195] = {
#include "stg07/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg07/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg07/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[141] = {
#include "stg07/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[75] = {
#include "stg07/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg07/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg07/terrain/draw_0.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg07/terrain/collision_46.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg07/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg07/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg07/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg07/terrain/collision_53.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg07/terrain/collision_57.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg07/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg07/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg07/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg07/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg07/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg07/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg07/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg07/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg07/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg07/terrain/draw_4.inc"
};
