/* STG37.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between background_archive.c and
 * object_archives.c, and its D lies between core_rouge.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg37/core_rouge.h"

#include "stg37/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[71];
struct land_table core_rouge_land;
static NJS_TEXNAME textures[25];
NJS_TEXLIST core_rouge_land_texlist;
static Sint16 draw_46_polygons[611];
static Sint32 draw_46_vertices[475];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_44_polygons[21];
static Sint32 draw_44_vertices[35];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_42_polygons[689];
static Sint32 draw_42_vertices[867];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_41_polygons[1003];
static Sint32 draw_41_vertices[699];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_39_polygons[25];
static Sint32 draw_39_vertices[35];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_38_polygons[407];
static Sint32 draw_38_vertices[323];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[233];
static Sint32 draw_37_vertices[199];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[141];
static Sint32 draw_36_vertices[131];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[691];
static Sint32 draw_35_vertices[531];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[893];
static Sint32 draw_34_vertices[835];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[43];
static Sint32 draw_33_vertices[35];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[227];
static Sint32 draw_31_vertices[139];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_27_polygons[217];
static Sint32 draw_27_vertices[243];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_25_polygons[875];
static Sint32 draw_25_vertices[839];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[547];
static Sint32 draw_24_vertices[307];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[907];
static Sint32 draw_23_vertices[483];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[1489];
static Sint32 draw_22_vertices[823];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[2069];
static Sint32 draw_21_vertices[1139];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_19_polygons[491];
static Sint32 draw_19_vertices[379];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_17_polygons[521];
static Sint32 draw_17_vertices[411];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_16_polygons[89];
static Sint32 draw_16_vertices[51];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_12_polygons[121];
static Sint32 draw_12_vertices[67];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_11_polygons[51];
static Sint32 draw_11_vertices[47];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_7_polygons[427];
static Sint32 draw_7_vertices[435];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_3_polygons[427];
static Sint32 draw_3_vertices[435];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_0_polygons[947];
static Sint32 draw_0_vertices[995];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[38];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[24];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[88];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[40];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[33];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[14];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[76];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[40];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[69];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[32];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[209];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[96];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[38];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[24];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[19];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[16];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[72];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[32];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[123];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[47];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[38];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[24];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[38];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[24];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[223];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[82];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[17];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[9];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[11];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[199];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[96];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[84];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[33];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[44];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[25];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_0;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;

static struct land_entry entries[71] = {
#include "stg37/terrain/entries.inc"
};
struct land_table core_rouge_land = {
#include "stg37/terrain/core_rouge_land.inc"
};
static NJS_TEXNAME textures[25] = {
#include "stg37/terrain/textures.inc"
};
NJS_TEXLIST core_rouge_land_texlist = {
#include "stg37/terrain/core_rouge_land_texlist.inc"
};
static Sint16 draw_46_polygons[611] = {
#include "stg37/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[475] = {
#include "stg37/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg37/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg37/terrain/draw_46.inc"
};
static Sint16 draw_44_polygons[21] = {
#include "stg37/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[35] = {
#include "stg37/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg37/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg37/terrain/draw_45.inc"
};
static Sint16 draw_42_polygons[689] = {
#include "stg37/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[867] = {
#include "stg37/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg37/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg37/terrain/draw_43.inc"
};
static Sint16 draw_41_polygons[1003] = {
#include "stg37/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[699] = {
#include "stg37/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg37/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg37/terrain/draw_41.inc"
};
static Sint16 draw_39_polygons[25] = {
#include "stg37/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[35] = {
#include "stg37/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg37/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg37/terrain/draw_40.inc"
};
static Sint16 draw_38_polygons[407] = {
#include "stg37/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[323] = {
#include "stg37/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg37/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg37/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[233] = {
#include "stg37/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[199] = {
#include "stg37/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg37/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg37/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[141] = {
#include "stg37/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[131] = {
#include "stg37/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg37/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg37/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[691] = {
#include "stg37/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[531] = {
#include "stg37/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg37/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg37/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[893] = {
#include "stg37/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[835] = {
#include "stg37/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg37/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg37/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[43] = {
#include "stg37/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[35] = {
#include "stg37/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg37/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg37/terrain/draw_33.inc"
};
static Sint16 draw_31_polygons[227] = {
#include "stg37/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[139] = {
#include "stg37/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg37/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg37/terrain/draw_32.inc"
};
static Sint16 draw_27_polygons[217] = {
#include "stg37/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[243] = {
#include "stg37/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg37/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg37/terrain/draw_28.inc"
};
static Sint16 draw_25_polygons[875] = {
#include "stg37/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[839] = {
#include "stg37/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg37/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg37/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[547] = {
#include "stg37/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[307] = {
#include "stg37/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg37/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg37/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[907] = {
#include "stg37/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[483] = {
#include "stg37/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg37/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg37/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[1489] = {
#include "stg37/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[823] = {
#include "stg37/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg37/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg37/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[2069] = {
#include "stg37/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[1139] = {
#include "stg37/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg37/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg37/terrain/draw_21.inc"
};
static Sint16 draw_19_polygons[491] = {
#include "stg37/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[379] = {
#include "stg37/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg37/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg37/terrain/draw_19.inc"
};
static Sint16 draw_17_polygons[521] = {
#include "stg37/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[411] = {
#include "stg37/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg37/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg37/terrain/draw_18.inc"
};
static Sint16 draw_16_polygons[89] = {
#include "stg37/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[51] = {
#include "stg37/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg37/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg37/terrain/draw_16.inc"
};
static Sint16 draw_12_polygons[121] = {
#include "stg37/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[67] = {
#include "stg37/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg37/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg37/terrain/draw_15.inc"
};
static Sint16 draw_11_polygons[51] = {
#include "stg37/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[47] = {
#include "stg37/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg37/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg37/terrain/draw_11.inc"
};
static Sint16 draw_7_polygons[427] = {
#include "stg37/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[435] = {
#include "stg37/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg37/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg37/terrain/draw_8.inc"
};
static Sint16 draw_3_polygons[427] = {
#include "stg37/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[435] = {
#include "stg37/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg37/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg37/terrain/draw_6.inc"
};
static Sint16 draw_0_polygons[947] = {
#include "stg37/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[995] = {
#include "stg37/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg37/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg37/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg37/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[38] = {
#include "stg37/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg37/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[24] = {
#include "stg37/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg37/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg37/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg37/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[88] = {
#include "stg37/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg37/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[40] = {
#include "stg37/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg37/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg37/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg37/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[33] = {
#include "stg37/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg37/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[14] = {
#include "stg37/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg37/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg37/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg37/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[76] = {
#include "stg37/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg37/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[40] = {
#include "stg37/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg37/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg37/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg37/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[69] = {
#include "stg37/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg37/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[32] = {
#include "stg37/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg37/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg37/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg37/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[209] = {
#include "stg37/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg37/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[96] = {
#include "stg37/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg37/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg37/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg37/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[38] = {
#include "stg37/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg37/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[24] = {
#include "stg37/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg37/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg37/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg37/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[19] = {
#include "stg37/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg37/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[16] = {
#include "stg37/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg37/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg37/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg37/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[72] = {
#include "stg37/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg37/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[32] = {
#include "stg37/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg37/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg37/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg37/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[123] = {
#include "stg37/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg37/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[47] = {
#include "stg37/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg37/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg37/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg37/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[38] = {
#include "stg37/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg37/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[24] = {
#include "stg37/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg37/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg37/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg37/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[38] = {
#include "stg37/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg37/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[24] = {
#include "stg37/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg37/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg37/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg37/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[223] = {
#include "stg37/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg37/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[82] = {
#include "stg37/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg37/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg37/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg37/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[17] = {
#include "stg37/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg37/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[9] = {
#include "stg37/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg37/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg37/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg37/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[11] = {
#include "stg37/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg37/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg37/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg37/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg37/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg37/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[199] = {
#include "stg37/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg37/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[96] = {
#include "stg37/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg37/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg37/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg37/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[84] = {
#include "stg37/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg37/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[33] = {
#include "stg37/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg37/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg37/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg37/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[44] = {
#include "stg37/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg37/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[25] = {
#include "stg37/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg37/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg37/terrain/collision_23.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg37/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg37/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg37/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg37/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg37/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg37/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg37/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg37/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg37/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg37/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg37/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg37/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg37/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg37/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg37/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg37/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg37/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg37/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg37/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg37/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg37/terrain/draw_1.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg37/terrain/collision_0.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg37/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg37/terrain/collision_3.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg37/terrain/collision_6.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg37/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg37/terrain/collision_9.inc"
};
