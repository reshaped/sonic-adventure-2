#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "shared/carrier_land.h"

/* The carrier arena names its terrain archive and textures in this pool. */
const char carrier_land_filenames[381] = {
#include "stg15/land/carrier_land_filenames.inc"
};

/* STG15.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between weapons_bed_2p.c and object_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[77];
struct land_table carrier_land;
static NJS_TEXNAME textures[36];
NJS_TEXLIST carrier_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[7];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[6];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static Sint16 draw_57_polygons[93];
static Sint32 draw_57_vertices[39];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_56_polygons[169];
static Sint32 draw_56_vertices[195];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[185];
static Sint32 draw_55_vertices[123];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_53_polygons[381];
static Sint32 draw_53_vertices[187];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_52_polygons[559];
static Sint32 draw_52_vertices[259];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_49_polygons[89];
static Sint32 draw_49_vertices[35];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_48_polygons[261];
static Sint32 draw_48_vertices[151];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_46_polygons[25];
static Sint32 draw_46_vertices[19];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_45_polygons[49];
static Sint32 draw_45_vertices[51];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_44_polygons[341];
static Sint32 draw_44_vertices[403];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[125];
static Sint32 draw_43_vertices[51];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[169];
static Sint32 draw_42_vertices[103];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[39];
static Sint32 draw_41_vertices[35];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[81];
static Sint32 draw_40_vertices[35];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[161];
static Sint32 draw_39_vertices[83];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[45];
static Sint32 draw_38_vertices[35];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[209];
static Sint32 draw_37_vertices[99];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[125];
static Sint32 draw_36_vertices[63];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[171];
static Sint32 draw_35_vertices[99];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[225];
static Sint32 draw_34_vertices[147];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[147];
static Sint32 draw_33_vertices[75];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_31_polygons[25];
static Sint32 draw_31_vertices[19];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_30_polygons[65];
static Sint32 draw_30_vertices[39];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[49];
static Sint32 draw_29_vertices[51];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[37];
static Sint32 draw_28_vertices[35];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[59];
static Sint32 draw_27_vertices[51];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[61];
static Sint32 draw_26_vertices[67];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_25_polygons[49];
static Sint32 draw_25_vertices[51];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[49];
static Sint32 draw_24_vertices[51];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_20_polygons[83];
static Sint32 draw_20_vertices[55];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_19_polygons[189];
static Sint32 draw_19_vertices[83];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[61];
static Sint32 draw_18_vertices[27];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_16_polygons[223];
static Sint32 draw_16_vertices[99];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_15_polygons[263];
static Sint32 draw_15_vertices[127];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_11_polygons[159];
static Sint32 draw_11_vertices[67];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_10_polygons[1029];
static Sint32 draw_10_vertices[483];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[63];
static Sint32 draw_9_vertices[35];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[537];
static Sint32 draw_8_vertices[227];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_6_polygons[241];
static Sint32 draw_6_vertices[131];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_5_polygons[93];
static Sint32 draw_5_vertices[75];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[405];
static Sint32 draw_4_vertices[227];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[169];
static Sint32 draw_3_vertices[103];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_1_polygons[57];
static Sint32 draw_1_vertices[31];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_2;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[15];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[8];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[42];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[26];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[30];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[21];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[30];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[16];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[19];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[8];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[45];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[23];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static Sint16 draw_0_polygons[61];
static Sint32 draw_0_vertices[35];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[57];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[35];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[28];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[15];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[31];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[23];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[41];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[22];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[27];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[16];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[44];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[20];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_9;

static struct land_entry entries[77] = {
#include "stg15/land/entries.inc"
};
struct land_table carrier_land = {
#include "stg15/land/carrier_land.inc"
};
static NJS_TEXNAME textures[36] = {
#include "stg15/land/textures.inc"
};
NJS_TEXLIST carrier_land_texlist = {
#include "stg15/land/carrier_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg15/land/collision_0_materials.inc"
};
static Sint16 collision_0_faces[7] = {
#include "stg15/land/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg15/land/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[6] = {
#include "stg15/land/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg15/land/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg15/land/collision_0.inc"
};
static Sint16 draw_57_polygons[93] = {
#include "stg15/land/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[39] = {
#include "stg15/land/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg15/land/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg15/land/draw_58.inc"
};
static Sint16 draw_56_polygons[169] = {
#include "stg15/land/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[195] = {
#include "stg15/land/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg15/land/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg15/land/draw_56.inc"
};
static Sint16 draw_55_polygons[185] = {
#include "stg15/land/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[123] = {
#include "stg15/land/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg15/land/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg15/land/draw_55.inc"
};
static Sint16 draw_53_polygons[381] = {
#include "stg15/land/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[187] = {
#include "stg15/land/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg15/land/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg15/land/draw_54.inc"
};
static Sint16 draw_52_polygons[559] = {
#include "stg15/land/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[259] = {
#include "stg15/land/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg15/land/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg15/land/draw_52.inc"
};
static Sint16 draw_49_polygons[89] = {
#include "stg15/land/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[35] = {
#include "stg15/land/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg15/land/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg15/land/draw_51.inc"
};
static Sint16 draw_48_polygons[261] = {
#include "stg15/land/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[151] = {
#include "stg15/land/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg15/land/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg15/land/draw_48.inc"
};
static Sint16 draw_46_polygons[25] = {
#include "stg15/land/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[19] = {
#include "stg15/land/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg15/land/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg15/land/draw_47.inc"
};
static Sint16 draw_45_polygons[49] = {
#include "stg15/land/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[51] = {
#include "stg15/land/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg15/land/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg15/land/draw_45.inc"
};
static Sint16 draw_44_polygons[341] = {
#include "stg15/land/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[403] = {
#include "stg15/land/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg15/land/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg15/land/draw_44.inc"
};
static Sint16 draw_43_polygons[125] = {
#include "stg15/land/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[51] = {
#include "stg15/land/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg15/land/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg15/land/draw_43.inc"
};
static Sint16 draw_42_polygons[169] = {
#include "stg15/land/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[103] = {
#include "stg15/land/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg15/land/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg15/land/draw_42.inc"
};
static Sint16 draw_41_polygons[39] = {
#include "stg15/land/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[35] = {
#include "stg15/land/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg15/land/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg15/land/draw_41.inc"
};
static Sint16 draw_40_polygons[81] = {
#include "stg15/land/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[35] = {
#include "stg15/land/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg15/land/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg15/land/draw_40.inc"
};
static Sint16 draw_39_polygons[161] = {
#include "stg15/land/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[83] = {
#include "stg15/land/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg15/land/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg15/land/draw_39.inc"
};
static Sint16 draw_38_polygons[45] = {
#include "stg15/land/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[35] = {
#include "stg15/land/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg15/land/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg15/land/draw_38.inc"
};
static Sint16 draw_37_polygons[209] = {
#include "stg15/land/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[99] = {
#include "stg15/land/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg15/land/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg15/land/draw_37.inc"
};
static Sint16 draw_36_polygons[125] = {
#include "stg15/land/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[63] = {
#include "stg15/land/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg15/land/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg15/land/draw_36.inc"
};
static Sint16 draw_35_polygons[171] = {
#include "stg15/land/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[99] = {
#include "stg15/land/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg15/land/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg15/land/draw_35.inc"
};
static Sint16 draw_34_polygons[225] = {
#include "stg15/land/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[147] = {
#include "stg15/land/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg15/land/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg15/land/draw_34.inc"
};
static Sint16 draw_33_polygons[147] = {
#include "stg15/land/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[75] = {
#include "stg15/land/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg15/land/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg15/land/draw_33.inc"
};
static Sint16 draw_31_polygons[25] = {
#include "stg15/land/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[19] = {
#include "stg15/land/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg15/land/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg15/land/draw_32.inc"
};
static Sint16 draw_30_polygons[65] = {
#include "stg15/land/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[39] = {
#include "stg15/land/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg15/land/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg15/land/draw_30.inc"
};
static Sint16 draw_29_polygons[49] = {
#include "stg15/land/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[51] = {
#include "stg15/land/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg15/land/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg15/land/draw_29.inc"
};
static Sint16 draw_28_polygons[37] = {
#include "stg15/land/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[35] = {
#include "stg15/land/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg15/land/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg15/land/draw_28.inc"
};
static Sint16 draw_27_polygons[59] = {
#include "stg15/land/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[51] = {
#include "stg15/land/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg15/land/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg15/land/draw_27.inc"
};
static Sint16 draw_26_polygons[61] = {
#include "stg15/land/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[67] = {
#include "stg15/land/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg15/land/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg15/land/draw_26.inc"
};
static Sint16 draw_25_polygons[49] = {
#include "stg15/land/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[51] = {
#include "stg15/land/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg15/land/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg15/land/draw_25.inc"
};
static Sint16 draw_24_polygons[49] = {
#include "stg15/land/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[51] = {
#include "stg15/land/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg15/land/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg15/land/draw_24.inc"
};
static Sint16 draw_20_polygons[83] = {
#include "stg15/land/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[55] = {
#include "stg15/land/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg15/land/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg15/land/draw_23.inc"
};
static Sint16 draw_19_polygons[189] = {
#include "stg15/land/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[83] = {
#include "stg15/land/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg15/land/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg15/land/draw_19.inc"
};
static Sint16 draw_18_polygons[61] = {
#include "stg15/land/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[27] = {
#include "stg15/land/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg15/land/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg15/land/draw_18.inc"
};
static Sint16 draw_16_polygons[223] = {
#include "stg15/land/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[99] = {
#include "stg15/land/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg15/land/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg15/land/draw_17.inc"
};
static Sint16 draw_15_polygons[263] = {
#include "stg15/land/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[127] = {
#include "stg15/land/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg15/land/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg15/land/draw_15.inc"
};
static Sint16 draw_11_polygons[159] = {
#include "stg15/land/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[67] = {
#include "stg15/land/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg15/land/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg15/land/draw_14.inc"
};
static Sint16 draw_10_polygons[1029] = {
#include "stg15/land/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[483] = {
#include "stg15/land/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg15/land/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg15/land/draw_10.inc"
};
static Sint16 draw_9_polygons[63] = {
#include "stg15/land/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[35] = {
#include "stg15/land/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg15/land/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg15/land/draw_9.inc"
};
static Sint16 draw_8_polygons[537] = {
#include "stg15/land/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[227] = {
#include "stg15/land/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg15/land/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg15/land/draw_8.inc"
};
static Sint16 draw_6_polygons[241] = {
#include "stg15/land/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[131] = {
#include "stg15/land/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg15/land/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg15/land/draw_7.inc"
};
static Sint16 draw_5_polygons[93] = {
#include "stg15/land/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[75] = {
#include "stg15/land/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg15/land/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg15/land/draw_5.inc"
};
static Sint16 draw_4_polygons[405] = {
#include "stg15/land/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[227] = {
#include "stg15/land/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg15/land/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg15/land/draw_4.inc"
};
static Sint16 draw_3_polygons[169] = {
#include "stg15/land/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[103] = {
#include "stg15/land/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg15/land/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg15/land/draw_3.inc"
};
static Sint16 draw_1_polygons[57] = {
#include "stg15/land/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[31] = {
#include "stg15/land/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg15/land/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg15/land/draw_2.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg15/land/collision_1_materials.inc"
};
static Sint16 collision_1_faces[15] = {
#include "stg15/land/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg15/land/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[8] = {
#include "stg15/land/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg15/land/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg15/land/collision_1.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg15/land/collision_3_materials.inc"
};
static Sint16 collision_3_faces[42] = {
#include "stg15/land/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg15/land/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[26] = {
#include "stg15/land/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg15/land/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg15/land/collision_3.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg15/land/collision_2_materials.inc"
};
static Sint16 collision_2_faces[30] = {
#include "stg15/land/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg15/land/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[21] = {
#include "stg15/land/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg15/land/collision_2_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg15/land/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg15/land/collision_5_materials.inc"
};
static Sint16 collision_5_faces[30] = {
#include "stg15/land/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg15/land/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[16] = {
#include "stg15/land/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg15/land/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg15/land/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg15/land/collision_6_materials.inc"
};
static Sint16 collision_6_faces[19] = {
#include "stg15/land/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg15/land/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[8] = {
#include "stg15/land/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg15/land/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg15/land/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg15/land/collision_8_materials.inc"
};
static Sint16 collision_8_faces[45] = {
#include "stg15/land/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg15/land/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[23] = {
#include "stg15/land/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg15/land/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg15/land/collision_8.inc"
};
static Sint16 draw_0_polygons[61] = {
#include "stg15/land/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[35] = {
#include "stg15/land/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg15/land/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg15/land/draw_0.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg15/land/collision_10_materials.inc"
};
static Sint16 collision_10_faces[57] = {
#include "stg15/land/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg15/land/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[35] = {
#include "stg15/land/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg15/land/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg15/land/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg15/land/collision_11_materials.inc"
};
static Sint16 collision_11_faces[28] = {
#include "stg15/land/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg15/land/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[15] = {
#include "stg15/land/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg15/land/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg15/land/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg15/land/collision_12_materials.inc"
};
static Sint16 collision_12_faces[31] = {
#include "stg15/land/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg15/land/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[23] = {
#include "stg15/land/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg15/land/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg15/land/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg15/land/collision_13_materials.inc"
};
static Sint16 collision_13_faces[41] = {
#include "stg15/land/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg15/land/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[22] = {
#include "stg15/land/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg15/land/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg15/land/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg15/land/collision_14_materials.inc"
};
static Sint16 collision_14_faces[27] = {
#include "stg15/land/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg15/land/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[16] = {
#include "stg15/land/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg15/land/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg15/land/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg15/land/collision_15_materials.inc"
};
static Sint16 collision_15_faces[44] = {
#include "stg15/land/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg15/land/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[20] = {
#include "stg15/land/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg15/land/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg15/land/collision_15.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg15/land/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg15/land/draw_59.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg15/land/draw_57.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg15/land/draw_53.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg15/land/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg15/land/draw_49.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg15/land/draw_46.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg15/land/draw_31.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg15/land/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg15/land/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg15/land/draw_20.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg15/land/draw_16.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg15/land/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg15/land/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg15/land/draw_11.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg15/land/draw_6.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg15/land/draw_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg15/land/collision_2.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg15/land/collision_7.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg15/land/collision_9.inc"
};
