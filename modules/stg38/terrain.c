/* STG38.PRS's land, texture lists, chunk models, Basic models and texture
 * names, from the disc. its C lies between background_archive.c and
 * object_archives.c, and its D lies between core_knuckles.c and
 * object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg38/core_knuckles.h"

#include "stg38/terrain/text.inc"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[126];
struct land_table core_knuckles_land;
static NJS_TEXNAME textures[31];
NJS_TEXLIST core_knuckles_land_texlist;
static Sint16 draw_82_polygons[127];
static Sint32 draw_82_vertices[99];
static NJS_CNK_MODEL draw_82_model;
static NJS_CNK_OBJECT draw_83;
static Sint16 draw_78_polygons[427];
static Sint32 draw_78_vertices[435];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_81;
static Sint16 draw_74_polygons[427];
static Sint32 draw_74_vertices[435];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_70_polygons[947];
static Sint32 draw_70_vertices[995];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_69_polygons[875];
static Sint32 draw_69_vertices[839];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[2567];
static Sint32 draw_68_vertices[2707];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[547];
static Sint32 draw_67_vertices[307];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[907];
static Sint32 draw_66_vertices[483];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[1489];
static Sint32 draw_65_vertices[823];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[2069];
static Sint32 draw_64_vertices[1139];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_61_polygons[447];
static Sint32 draw_61_vertices[347];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_60_polygons[521];
static Sint32 draw_60_vertices[411];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[691];
static Sint32 draw_59_vertices[531];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[899];
static Sint32 draw_58_vertices[835];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[43];
static Sint32 draw_57_vertices[35];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_53_polygons[227];
static Sint32 draw_53_vertices[139];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_45_polygons[217];
static Sint32 draw_45_vertices[243];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_43_polygons[35];
static Sint32 draw_43_vertices[67];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_39_polygons[167];
static Sint32 draw_39_vertices[155];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_38_polygons[611];
static Sint32 draw_38_vertices[475];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[835];
static Sint32 draw_37_vertices[651];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_35_polygons[31];
static Sint32 draw_35_vertices[51];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_34_polygons[207];
static Sint32 draw_34_vertices[127];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_29_polygons[689];
static Sint32 draw_29_vertices[867];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_28_polygons[689];
static Sint32 draw_28_vertices[867];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_22_polygons[25];
static Sint32 draw_22_vertices[35];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_21_polygons[1003];
static Sint32 draw_21_vertices[699];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[959];
static Sint32 draw_20_vertices[667];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[989];
static Sint32 draw_19_vertices[711];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[407];
static Sint32 draw_18_vertices[323];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[407];
static Sint32 draw_17_vertices[307];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[407];
static Sint32 draw_16_vertices[323];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_15_polygons[335];
static Sint32 draw_15_vertices[371];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[393];
static Sint32 draw_14_vertices[323];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[575];
static Sint32 draw_13_vertices[515];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_11_polygons[1649];
static Sint32 draw_11_vertices[1555];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_9_polygons[325];
static Sint32 draw_9_vertices[979];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_8_polygons[339];
static Sint32 draw_8_vertices[267];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[213];
static Sint32 draw_7_vertices[195];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_5_polygons[39];
static Sint32 draw_5_vertices[67];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_4_polygons[93];
static Sint32 draw_4_vertices[67];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_2_polygons[407];
static Sint32 draw_2_vertices[327];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_1_polygons[289];
static Sint32 draw_1_vertices[223];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[107];
static Sint32 draw_0_vertices[75];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[38];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[22];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[12];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[49];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[20];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[324];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[124];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[44];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[20];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[205];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[96];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[105];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[64];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[140];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[56];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[38];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[24];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[19];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[16];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[72];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[32];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[123];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[47];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[150];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[65];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[38];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[24];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[38];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[24];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[107];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[48];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[225];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[77];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[232];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[85];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[210];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[77];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[108];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[60];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[106];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[42];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_30_materials[1];
static Sint16 collision_30_faces[98];
static NJS_MESHSET collision_30_meshsets[1];
static NJS_POINT3 collision_30_points[40];
static NJS_MODEL collision_30_model;
static NJS_OBJECT collision_30;
static NJS_MATERIAL collision_31_materials[1];
static Sint16 collision_31_faces[75];
static NJS_MESHSET collision_31_meshsets[1];
static NJS_POINT3 collision_31_points[30];
static NJS_MODEL collision_31_model;
static NJS_OBJECT collision_31;
static NJS_MATERIAL collision_32_materials[1];
static Sint16 collision_32_faces[84];
static NJS_MESHSET collision_32_meshsets[1];
static NJS_POINT3 collision_32_points[32];
static NJS_MODEL collision_32_model;
static NJS_OBJECT collision_32;
static NJS_MATERIAL collision_33_materials[1];
static Sint16 collision_33_faces[44];
static NJS_MESHSET collision_33_meshsets[1];
static NJS_POINT3 collision_33_points[32];
static NJS_MODEL collision_33_model;
static NJS_OBJECT collision_33;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[87];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[34];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_36_materials[1];
static Sint16 collision_36_faces[20];
static NJS_MESHSET collision_36_meshsets[1];
static NJS_POINT3 collision_36_points[9];
static NJS_MODEL collision_36_model;
static NJS_OBJECT collision_36;
static NJS_MATERIAL collision_38_materials[1];
static Sint16 collision_38_faces[40];
static NJS_MESHSET collision_38_meshsets[1];
static NJS_POINT3 collision_38_points[21];
static NJS_MODEL collision_38_model;
static NJS_OBJECT collision_38;
static NJS_MATERIAL collision_39_materials[1];
static Sint16 collision_39_faces[24];
static NJS_MESHSET collision_39_meshsets[1];
static NJS_POINT3 collision_39_points[14];
static NJS_MODEL collision_39_model;
static NJS_OBJECT collision_39;
static NJS_MATERIAL collision_40_materials[1];
static Sint16 collision_40_faces[31];
static NJS_MESHSET collision_40_meshsets[1];
static NJS_POINT3 collision_40_points[15];
static NJS_MODEL collision_40_model;
static NJS_OBJECT collision_40;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[26];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[14];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_2;
static NJS_OBJECT collision_0;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_4;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_17;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_37;

static struct land_entry entries[126] = {
#include "stg38/terrain/entries.inc"
};
struct land_table core_knuckles_land = {
#include "stg38/terrain/core_knuckles_land.inc"
};
static NJS_TEXNAME textures[31] = {
#include "stg38/terrain/textures.inc"
};
NJS_TEXLIST core_knuckles_land_texlist = {
#include "stg38/terrain/core_knuckles_land_texlist.inc"
};
static Sint16 draw_82_polygons[127] = {
#include "stg38/terrain/draw_82_polygons.inc"
};
static Sint32 draw_82_vertices[99] = {
#include "stg38/terrain/draw_82_vertices.inc"
};
static NJS_CNK_MODEL draw_82_model = {
#include "stg38/terrain/draw_82_model.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg38/terrain/draw_83.inc"
};
static Sint16 draw_78_polygons[427] = {
#include "stg38/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[435] = {
#include "stg38/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg38/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg38/terrain/draw_81.inc"
};
static Sint16 draw_74_polygons[427] = {
#include "stg38/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[435] = {
#include "stg38/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg38/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg38/terrain/draw_77.inc"
};
static Sint16 draw_70_polygons[947] = {
#include "stg38/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[995] = {
#include "stg38/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg38/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg38/terrain/draw_73.inc"
};
static Sint16 draw_69_polygons[875] = {
#include "stg38/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[839] = {
#include "stg38/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg38/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg38/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[2567] = {
#include "stg38/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[2707] = {
#include "stg38/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg38/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg38/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[547] = {
#include "stg38/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[307] = {
#include "stg38/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg38/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg38/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[907] = {
#include "stg38/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[483] = {
#include "stg38/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg38/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg38/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[1489] = {
#include "stg38/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[823] = {
#include "stg38/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg38/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg38/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[2069] = {
#include "stg38/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[1139] = {
#include "stg38/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg38/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg38/terrain/draw_64.inc"
};
static Sint16 draw_61_polygons[447] = {
#include "stg38/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[347] = {
#include "stg38/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg38/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg38/terrain/draw_63.inc"
};
static Sint16 draw_60_polygons[521] = {
#include "stg38/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[411] = {
#include "stg38/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg38/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg38/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[691] = {
#include "stg38/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[531] = {
#include "stg38/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg38/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg38/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[899] = {
#include "stg38/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[835] = {
#include "stg38/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg38/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg38/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[43] = {
#include "stg38/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[35] = {
#include "stg38/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg38/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg38/terrain/draw_57.inc"
};
static Sint16 draw_53_polygons[227] = {
#include "stg38/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[139] = {
#include "stg38/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg38/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg38/terrain/draw_56.inc"
};
static Sint16 draw_45_polygons[217] = {
#include "stg38/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[243] = {
#include "stg38/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg38/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg38/terrain/draw_50.inc"
};
static Sint16 draw_43_polygons[35] = {
#include "stg38/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[67] = {
#include "stg38/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg38/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg38/terrain/draw_44.inc"
};
static Sint16 draw_39_polygons[167] = {
#include "stg38/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[155] = {
#include "stg38/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg38/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg38/terrain/draw_42.inc"
};
static Sint16 draw_38_polygons[611] = {
#include "stg38/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[475] = {
#include "stg38/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg38/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg38/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[835] = {
#include "stg38/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[651] = {
#include "stg38/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg38/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg38/terrain/draw_37.inc"
};
static Sint16 draw_35_polygons[31] = {
#include "stg38/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[51] = {
#include "stg38/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg38/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg38/terrain/draw_36.inc"
};
static Sint16 draw_34_polygons[207] = {
#include "stg38/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[127] = {
#include "stg38/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg38/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg38/terrain/draw_34.inc"
};
static Sint16 draw_29_polygons[689] = {
#include "stg38/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[867] = {
#include "stg38/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg38/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg38/terrain/draw_33.inc"
};
static Sint16 draw_28_polygons[689] = {
#include "stg38/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[867] = {
#include "stg38/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg38/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg38/terrain/draw_28.inc"
};
static Sint16 draw_22_polygons[25] = {
#include "stg38/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[35] = {
#include "stg38/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg38/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg38/terrain/draw_27.inc"
};
static Sint16 draw_21_polygons[1003] = {
#include "stg38/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[699] = {
#include "stg38/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg38/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg38/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[959] = {
#include "stg38/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[667] = {
#include "stg38/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg38/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg38/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[989] = {
#include "stg38/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[711] = {
#include "stg38/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg38/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg38/terrain/draw_19.inc"
};
static Sint16 draw_18_polygons[407] = {
#include "stg38/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[323] = {
#include "stg38/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg38/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg38/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[407] = {
#include "stg38/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[307] = {
#include "stg38/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg38/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg38/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[407] = {
#include "stg38/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[323] = {
#include "stg38/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg38/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg38/terrain/draw_16.inc"
};
static Sint16 draw_15_polygons[335] = {
#include "stg38/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[371] = {
#include "stg38/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg38/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg38/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[393] = {
#include "stg38/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[323] = {
#include "stg38/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg38/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg38/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[575] = {
#include "stg38/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[515] = {
#include "stg38/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg38/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg38/terrain/draw_13.inc"
};
static Sint16 draw_11_polygons[1649] = {
#include "stg38/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[1555] = {
#include "stg38/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg38/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg38/terrain/draw_12.inc"
};
static Sint16 draw_9_polygons[325] = {
#include "stg38/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[979] = {
#include "stg38/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg38/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg38/terrain/draw_10.inc"
};
static Sint16 draw_8_polygons[339] = {
#include "stg38/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[267] = {
#include "stg38/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg38/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg38/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[213] = {
#include "stg38/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[195] = {
#include "stg38/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg38/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg38/terrain/draw_7.inc"
};
static Sint16 draw_5_polygons[39] = {
#include "stg38/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[67] = {
#include "stg38/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg38/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg38/terrain/draw_6.inc"
};
static Sint16 draw_4_polygons[93] = {
#include "stg38/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[67] = {
#include "stg38/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg38/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg38/terrain/draw_4.inc"
};
static Sint16 draw_2_polygons[407] = {
#include "stg38/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[327] = {
#include "stg38/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg38/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg38/terrain/draw_3.inc"
};
static Sint16 draw_1_polygons[289] = {
#include "stg38/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[223] = {
#include "stg38/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg38/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg38/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[107] = {
#include "stg38/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[75] = {
#include "stg38/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg38/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg38/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg38/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[38] = {
#include "stg38/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg38/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg38/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg38/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg38/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg38/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[22] = {
#include "stg38/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg38/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[12] = {
#include "stg38/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg38/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg38/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg38/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[49] = {
#include "stg38/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg38/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[20] = {
#include "stg38/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg38/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg38/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg38/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[324] = {
#include "stg38/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg38/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[124] = {
#include "stg38/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg38/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg38/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg38/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[44] = {
#include "stg38/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg38/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[20] = {
#include "stg38/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg38/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg38/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg38/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[205] = {
#include "stg38/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg38/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[96] = {
#include "stg38/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg38/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg38/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg38/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[105] = {
#include "stg38/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg38/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[64] = {
#include "stg38/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg38/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg38/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg38/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[140] = {
#include "stg38/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg38/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[56] = {
#include "stg38/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg38/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg38/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg38/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[38] = {
#include "stg38/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg38/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[24] = {
#include "stg38/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg38/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg38/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg38/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[19] = {
#include "stg38/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg38/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[16] = {
#include "stg38/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg38/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg38/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg38/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[72] = {
#include "stg38/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg38/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[32] = {
#include "stg38/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg38/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg38/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg38/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[123] = {
#include "stg38/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg38/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[47] = {
#include "stg38/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg38/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg38/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg38/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[150] = {
#include "stg38/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg38/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[65] = {
#include "stg38/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg38/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg38/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg38/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[38] = {
#include "stg38/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg38/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[24] = {
#include "stg38/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg38/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg38/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg38/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[38] = {
#include "stg38/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg38/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[24] = {
#include "stg38/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg38/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg38/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg38/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[107] = {
#include "stg38/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg38/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[48] = {
#include "stg38/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg38/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg38/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg38/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[225] = {
#include "stg38/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg38/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[77] = {
#include "stg38/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg38/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg38/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg38/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[232] = {
#include "stg38/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg38/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[85] = {
#include "stg38/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg38/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg38/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg38/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[210] = {
#include "stg38/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg38/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[77] = {
#include "stg38/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg38/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg38/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg38/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[108] = {
#include "stg38/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg38/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[60] = {
#include "stg38/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg38/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg38/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg38/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[106] = {
#include "stg38/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg38/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[42] = {
#include "stg38/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg38/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg38/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_30_materials[1] = {
#include "stg38/terrain/collision_30_materials.inc"
};
static Sint16 collision_30_faces[98] = {
#include "stg38/terrain/collision_30_faces.inc"
};
static NJS_MESHSET collision_30_meshsets[1] = {
#include "stg38/terrain/collision_30_meshsets.inc"
};
static NJS_POINT3 collision_30_points[40] = {
#include "stg38/terrain/collision_30_points.inc"
};
static NJS_MODEL collision_30_model = {
#include "stg38/terrain/collision_30_model.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg38/terrain/collision_30.inc"
};
static NJS_MATERIAL collision_31_materials[1] = {
#include "stg38/terrain/collision_31_materials.inc"
};
static Sint16 collision_31_faces[75] = {
#include "stg38/terrain/collision_31_faces.inc"
};
static NJS_MESHSET collision_31_meshsets[1] = {
#include "stg38/terrain/collision_31_meshsets.inc"
};
static NJS_POINT3 collision_31_points[30] = {
#include "stg38/terrain/collision_31_points.inc"
};
static NJS_MODEL collision_31_model = {
#include "stg38/terrain/collision_31_model.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg38/terrain/collision_31.inc"
};
static NJS_MATERIAL collision_32_materials[1] = {
#include "stg38/terrain/collision_32_materials.inc"
};
static Sint16 collision_32_faces[84] = {
#include "stg38/terrain/collision_32_faces.inc"
};
static NJS_MESHSET collision_32_meshsets[1] = {
#include "stg38/terrain/collision_32_meshsets.inc"
};
static NJS_POINT3 collision_32_points[32] = {
#include "stg38/terrain/collision_32_points.inc"
};
static NJS_MODEL collision_32_model = {
#include "stg38/terrain/collision_32_model.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg38/terrain/collision_32.inc"
};
static NJS_MATERIAL collision_33_materials[1] = {
#include "stg38/terrain/collision_33_materials.inc"
};
static Sint16 collision_33_faces[44] = {
#include "stg38/terrain/collision_33_faces.inc"
};
static NJS_MESHSET collision_33_meshsets[1] = {
#include "stg38/terrain/collision_33_meshsets.inc"
};
static NJS_POINT3 collision_33_points[32] = {
#include "stg38/terrain/collision_33_points.inc"
};
static NJS_MODEL collision_33_model = {
#include "stg38/terrain/collision_33_model.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg38/terrain/collision_33.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg38/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[87] = {
#include "stg38/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg38/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[34] = {
#include "stg38/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg38/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg38/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_36_materials[1] = {
#include "stg38/terrain/collision_36_materials.inc"
};
static Sint16 collision_36_faces[20] = {
#include "stg38/terrain/collision_36_faces.inc"
};
static NJS_MESHSET collision_36_meshsets[1] = {
#include "stg38/terrain/collision_36_meshsets.inc"
};
static NJS_POINT3 collision_36_points[9] = {
#include "stg38/terrain/collision_36_points.inc"
};
static NJS_MODEL collision_36_model = {
#include "stg38/terrain/collision_36_model.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg38/terrain/collision_36.inc"
};
static NJS_MATERIAL collision_38_materials[1] = {
#include "stg38/terrain/collision_38_materials.inc"
};
static Sint16 collision_38_faces[40] = {
#include "stg38/terrain/collision_38_faces.inc"
};
static NJS_MESHSET collision_38_meshsets[1] = {
#include "stg38/terrain/collision_38_meshsets.inc"
};
static NJS_POINT3 collision_38_points[21] = {
#include "stg38/terrain/collision_38_points.inc"
};
static NJS_MODEL collision_38_model = {
#include "stg38/terrain/collision_38_model.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg38/terrain/collision_38.inc"
};
static NJS_MATERIAL collision_39_materials[1] = {
#include "stg38/terrain/collision_39_materials.inc"
};
static Sint16 collision_39_faces[24] = {
#include "stg38/terrain/collision_39_faces.inc"
};
static NJS_MESHSET collision_39_meshsets[1] = {
#include "stg38/terrain/collision_39_meshsets.inc"
};
static NJS_POINT3 collision_39_points[14] = {
#include "stg38/terrain/collision_39_points.inc"
};
static NJS_MODEL collision_39_model = {
#include "stg38/terrain/collision_39_model.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg38/terrain/collision_39.inc"
};
static NJS_MATERIAL collision_40_materials[1] = {
#include "stg38/terrain/collision_40_materials.inc"
};
static Sint16 collision_40_faces[31] = {
#include "stg38/terrain/collision_40_faces.inc"
};
static NJS_MESHSET collision_40_meshsets[1] = {
#include "stg38/terrain/collision_40_meshsets.inc"
};
static NJS_POINT3 collision_40_points[15] = {
#include "stg38/terrain/collision_40_points.inc"
};
static NJS_MODEL collision_40_model = {
#include "stg38/terrain/collision_40_model.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg38/terrain/collision_40.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg38/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[26] = {
#include "stg38/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg38/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[14] = {
#include "stg38/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg38/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg38/terrain/collision_41.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg38/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg38/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg38/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg38/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg38/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg38/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg38/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg38/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg38/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg38/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg38/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg38/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg38/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg38/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg38/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg38/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg38/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg38/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg38/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg38/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg38/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg38/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg38/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg38/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg38/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg38/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg38/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg38/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg38/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg38/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg38/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg38/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg38/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg38/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg38/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg38/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg38/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg38/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg38/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg38/terrain/draw_2.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg38/terrain/collision_0.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg38/terrain/collision_3.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg38/terrain/collision_4.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg38/terrain/collision_7.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg38/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg38/terrain/collision_10.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg38/terrain/collision_17.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg38/terrain/collision_21.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg38/terrain/collision_23.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg38/terrain/collision_35.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg38/terrain/collision_37.inc"
};
