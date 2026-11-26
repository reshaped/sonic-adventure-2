#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg29/versus_mech_2.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char versus_mech_2_land_filenames[311] = {
#include "stg29/terrain/versus_mech_2_land_filenames.inc"
};

/* STG29.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between versus_mech_2.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[72];
struct land_table versus_mech_2_land;
static NJS_TEXNAME textures[25];
static NJS_TEXLIST terrain_textures;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[50];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[23];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[47];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[23];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[44];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[22];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[38];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[24];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static Sint16 draw_67_polygons[23];
static Sint32 draw_67_vertices[19];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[23];
static Sint32 draw_66_vertices[19];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[23];
static Sint32 draw_65_vertices[19];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_64_polygons[23];
static Sint32 draw_64_vertices[19];
static NJS_CNK_MODEL draw_64_model;
static NJS_CNK_OBJECT draw_64;
static Sint16 draw_63_polygons[23];
static Sint32 draw_63_vertices[19];
static NJS_CNK_MODEL draw_63_model;
static NJS_CNK_OBJECT draw_63;
static Sint16 draw_62_polygons[23];
static Sint32 draw_62_vertices[19];
static NJS_CNK_MODEL draw_62_model;
static NJS_CNK_OBJECT draw_62;
static Sint16 draw_61_polygons[23];
static Sint32 draw_61_vertices[19];
static NJS_CNK_MODEL draw_61_model;
static NJS_CNK_OBJECT draw_61;
static Sint16 draw_60_polygons[23];
static Sint32 draw_60_vertices[19];
static NJS_CNK_MODEL draw_60_model;
static NJS_CNK_OBJECT draw_60;
static Sint16 draw_59_polygons[203];
static Sint32 draw_59_vertices[151];
static NJS_CNK_MODEL draw_59_model;
static NJS_CNK_OBJECT draw_59;
static Sint16 draw_58_polygons[333];
static Sint32 draw_58_vertices[259];
static NJS_CNK_MODEL draw_58_model;
static NJS_CNK_OBJECT draw_58;
static Sint16 draw_57_polygons[23];
static Sint32 draw_57_vertices[19];
static NJS_CNK_MODEL draw_57_model;
static NJS_CNK_OBJECT draw_57;
static Sint16 draw_56_polygons[23];
static Sint32 draw_56_vertices[19];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[23];
static Sint32 draw_55_vertices[19];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_54_polygons[23];
static Sint32 draw_54_vertices[19];
static NJS_CNK_MODEL draw_54_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_53_polygons[23];
static Sint32 draw_53_vertices[19];
static NJS_CNK_MODEL draw_53_model;
static NJS_CNK_OBJECT draw_53;
static Sint16 draw_52_polygons[23];
static Sint32 draw_52_vertices[19];
static NJS_CNK_MODEL draw_52_model;
static NJS_CNK_OBJECT draw_52;
static Sint16 draw_51_polygons[23];
static Sint32 draw_51_vertices[19];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_51;
static Sint16 draw_50_polygons[23];
static Sint32 draw_50_vertices[19];
static NJS_CNK_MODEL draw_50_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_49_polygons[23];
static Sint32 draw_49_vertices[19];
static NJS_CNK_MODEL draw_49_model;
static NJS_CNK_OBJECT draw_49;
static Sint16 draw_48_polygons[333];
static Sint32 draw_48_vertices[259];
static NJS_CNK_MODEL draw_48_model;
static NJS_CNK_OBJECT draw_48;
static Sint16 draw_47_polygons[231];
static Sint32 draw_47_vertices[163];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_47;
static Sint16 draw_46_polygons[23];
static Sint32 draw_46_vertices[19];
static NJS_CNK_MODEL draw_46_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_45_polygons[23];
static Sint32 draw_45_vertices[19];
static NJS_CNK_MODEL draw_45_model;
static NJS_CNK_OBJECT draw_45;
static Sint16 draw_44_polygons[23];
static Sint32 draw_44_vertices[19];
static NJS_CNK_MODEL draw_44_model;
static NJS_CNK_OBJECT draw_44;
static Sint16 draw_43_polygons[23];
static Sint32 draw_43_vertices[19];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_43;
static Sint16 draw_42_polygons[23];
static Sint32 draw_42_vertices[19];
static NJS_CNK_MODEL draw_42_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_41_polygons[567];
static Sint32 draw_41_vertices[579];
static NJS_CNK_MODEL draw_41_model;
static NJS_CNK_OBJECT draw_41;
static Sint16 draw_40_polygons[553];
static Sint32 draw_40_vertices[563];
static NJS_CNK_MODEL draw_40_model;
static NJS_CNK_OBJECT draw_40;
static Sint16 draw_39_polygons[567];
static Sint32 draw_39_vertices[579];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_39;
static Sint16 draw_38_polygons[975];
static Sint32 draw_38_vertices[619];
static NJS_CNK_MODEL draw_38_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_37_polygons[223];
static Sint32 draw_37_vertices[159];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static Sint16 draw_36_polygons[975];
static Sint32 draw_36_vertices[619];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[371];
static Sint32 draw_35_vertices[339];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[1133];
static Sint32 draw_34_vertices[863];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[299];
static Sint32 draw_33_vertices[323];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_32_polygons[507];
static Sint32 draw_32_vertices[315];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static Sint16 draw_31_polygons[975];
static Sint32 draw_31_vertices[619];
static NJS_CNK_MODEL draw_31_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_30_polygons[1133];
static Sint32 draw_30_vertices[863];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_30;
static Sint16 draw_29_polygons[335];
static Sint32 draw_29_vertices[311];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[383];
static Sint32 draw_28_vertices[255];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_27_polygons[1583];
static Sint32 draw_27_vertices[931];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_26_polygons[231];
static Sint32 draw_26_vertices[167];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_22_polygons[101];
static Sint32 draw_22_vertices[99];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_21_polygons[2253];
static Sint32 draw_21_vertices[1499];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[1735];
static Sint32 draw_20_vertices[1455];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[345];
static Sint32 draw_19_vertices[171];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_18_polygons[373];
static Sint32 draw_18_vertices[295];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_17_polygons[365];
static Sint32 draw_17_vertices[295];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_16_polygons[383];
static Sint32 draw_16_vertices[255];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_12_polygons[101];
static Sint32 draw_12_vertices[99];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_11_polygons[101];
static Sint32 draw_11_vertices[99];
static NJS_CNK_MODEL draw_11_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_10_polygons[277];
static Sint32 draw_10_vertices[211];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_10;
static Sint16 draw_9_polygons[49];
static Sint32 draw_9_vertices[51];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[49];
static Sint32 draw_8_vertices[51];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[49];
static Sint32 draw_7_vertices[51];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_5_polygons[49];
static Sint32 draw_5_vertices[51];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_4_polygons[87];
static Sint32 draw_4_vertices[99];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[259];
static Sint32 draw_3_vertices[247];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[365];
static Sint32 draw_2_vertices[219];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[49];
static Sint32 draw_1_vertices[51];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[99];
static Sint32 draw_0_vertices[99];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_5;

static struct land_entry entries[72] = {
#include "stg29/terrain/entries.inc"
};
struct land_table versus_mech_2_land = {
#include "stg29/terrain/versus_mech_2_land.inc"
};
static NJS_TEXNAME textures[25] = {
#include "stg29/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "stg29/terrain/terrain_textures.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg29/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[50] = {
#include "stg29/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg29/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[23] = {
#include "stg29/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg29/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg29/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg29/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[47] = {
#include "stg29/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg29/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[23] = {
#include "stg29/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg29/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg29/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg29/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[44] = {
#include "stg29/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg29/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[22] = {
#include "stg29/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg29/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg29/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg29/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[38] = {
#include "stg29/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg29/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[24] = {
#include "stg29/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg29/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg29/terrain/collision_3.inc"
};
static Sint16 draw_67_polygons[23] = {
#include "stg29/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[19] = {
#include "stg29/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg29/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg29/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[23] = {
#include "stg29/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[19] = {
#include "stg29/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg29/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg29/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[23] = {
#include "stg29/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[19] = {
#include "stg29/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg29/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg29/terrain/draw_65.inc"
};
static Sint16 draw_64_polygons[23] = {
#include "stg29/terrain/draw_64_polygons.inc"
};
static Sint32 draw_64_vertices[19] = {
#include "stg29/terrain/draw_64_vertices.inc"
};
static NJS_CNK_MODEL draw_64_model = {
#include "stg29/terrain/draw_64_model.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg29/terrain/draw_64.inc"
};
static Sint16 draw_63_polygons[23] = {
#include "stg29/terrain/draw_63_polygons.inc"
};
static Sint32 draw_63_vertices[19] = {
#include "stg29/terrain/draw_63_vertices.inc"
};
static NJS_CNK_MODEL draw_63_model = {
#include "stg29/terrain/draw_63_model.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg29/terrain/draw_63.inc"
};
static Sint16 draw_62_polygons[23] = {
#include "stg29/terrain/draw_62_polygons.inc"
};
static Sint32 draw_62_vertices[19] = {
#include "stg29/terrain/draw_62_vertices.inc"
};
static NJS_CNK_MODEL draw_62_model = {
#include "stg29/terrain/draw_62_model.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg29/terrain/draw_62.inc"
};
static Sint16 draw_61_polygons[23] = {
#include "stg29/terrain/draw_61_polygons.inc"
};
static Sint32 draw_61_vertices[19] = {
#include "stg29/terrain/draw_61_vertices.inc"
};
static NJS_CNK_MODEL draw_61_model = {
#include "stg29/terrain/draw_61_model.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg29/terrain/draw_61.inc"
};
static Sint16 draw_60_polygons[23] = {
#include "stg29/terrain/draw_60_polygons.inc"
};
static Sint32 draw_60_vertices[19] = {
#include "stg29/terrain/draw_60_vertices.inc"
};
static NJS_CNK_MODEL draw_60_model = {
#include "stg29/terrain/draw_60_model.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg29/terrain/draw_60.inc"
};
static Sint16 draw_59_polygons[203] = {
#include "stg29/terrain/draw_59_polygons.inc"
};
static Sint32 draw_59_vertices[151] = {
#include "stg29/terrain/draw_59_vertices.inc"
};
static NJS_CNK_MODEL draw_59_model = {
#include "stg29/terrain/draw_59_model.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg29/terrain/draw_59.inc"
};
static Sint16 draw_58_polygons[333] = {
#include "stg29/terrain/draw_58_polygons.inc"
};
static Sint32 draw_58_vertices[259] = {
#include "stg29/terrain/draw_58_vertices.inc"
};
static NJS_CNK_MODEL draw_58_model = {
#include "stg29/terrain/draw_58_model.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg29/terrain/draw_58.inc"
};
static Sint16 draw_57_polygons[23] = {
#include "stg29/terrain/draw_57_polygons.inc"
};
static Sint32 draw_57_vertices[19] = {
#include "stg29/terrain/draw_57_vertices.inc"
};
static NJS_CNK_MODEL draw_57_model = {
#include "stg29/terrain/draw_57_model.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg29/terrain/draw_57.inc"
};
static Sint16 draw_56_polygons[23] = {
#include "stg29/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[19] = {
#include "stg29/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg29/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg29/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[23] = {
#include "stg29/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[19] = {
#include "stg29/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg29/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg29/terrain/draw_55.inc"
};
static Sint16 draw_54_polygons[23] = {
#include "stg29/terrain/draw_54_polygons.inc"
};
static Sint32 draw_54_vertices[19] = {
#include "stg29/terrain/draw_54_vertices.inc"
};
static NJS_CNK_MODEL draw_54_model = {
#include "stg29/terrain/draw_54_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg29/terrain/draw_54.inc"
};
static Sint16 draw_53_polygons[23] = {
#include "stg29/terrain/draw_53_polygons.inc"
};
static Sint32 draw_53_vertices[19] = {
#include "stg29/terrain/draw_53_vertices.inc"
};
static NJS_CNK_MODEL draw_53_model = {
#include "stg29/terrain/draw_53_model.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg29/terrain/draw_53.inc"
};
static Sint16 draw_52_polygons[23] = {
#include "stg29/terrain/draw_52_polygons.inc"
};
static Sint32 draw_52_vertices[19] = {
#include "stg29/terrain/draw_52_vertices.inc"
};
static NJS_CNK_MODEL draw_52_model = {
#include "stg29/terrain/draw_52_model.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg29/terrain/draw_52.inc"
};
static Sint16 draw_51_polygons[23] = {
#include "stg29/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[19] = {
#include "stg29/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg29/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg29/terrain/draw_51.inc"
};
static Sint16 draw_50_polygons[23] = {
#include "stg29/terrain/draw_50_polygons.inc"
};
static Sint32 draw_50_vertices[19] = {
#include "stg29/terrain/draw_50_vertices.inc"
};
static NJS_CNK_MODEL draw_50_model = {
#include "stg29/terrain/draw_50_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg29/terrain/draw_50.inc"
};
static Sint16 draw_49_polygons[23] = {
#include "stg29/terrain/draw_49_polygons.inc"
};
static Sint32 draw_49_vertices[19] = {
#include "stg29/terrain/draw_49_vertices.inc"
};
static NJS_CNK_MODEL draw_49_model = {
#include "stg29/terrain/draw_49_model.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg29/terrain/draw_49.inc"
};
static Sint16 draw_48_polygons[333] = {
#include "stg29/terrain/draw_48_polygons.inc"
};
static Sint32 draw_48_vertices[259] = {
#include "stg29/terrain/draw_48_vertices.inc"
};
static NJS_CNK_MODEL draw_48_model = {
#include "stg29/terrain/draw_48_model.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg29/terrain/draw_48.inc"
};
static Sint16 draw_47_polygons[231] = {
#include "stg29/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[163] = {
#include "stg29/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg29/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg29/terrain/draw_47.inc"
};
static Sint16 draw_46_polygons[23] = {
#include "stg29/terrain/draw_46_polygons.inc"
};
static Sint32 draw_46_vertices[19] = {
#include "stg29/terrain/draw_46_vertices.inc"
};
static NJS_CNK_MODEL draw_46_model = {
#include "stg29/terrain/draw_46_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg29/terrain/draw_46.inc"
};
static Sint16 draw_45_polygons[23] = {
#include "stg29/terrain/draw_45_polygons.inc"
};
static Sint32 draw_45_vertices[19] = {
#include "stg29/terrain/draw_45_vertices.inc"
};
static NJS_CNK_MODEL draw_45_model = {
#include "stg29/terrain/draw_45_model.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg29/terrain/draw_45.inc"
};
static Sint16 draw_44_polygons[23] = {
#include "stg29/terrain/draw_44_polygons.inc"
};
static Sint32 draw_44_vertices[19] = {
#include "stg29/terrain/draw_44_vertices.inc"
};
static NJS_CNK_MODEL draw_44_model = {
#include "stg29/terrain/draw_44_model.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg29/terrain/draw_44.inc"
};
static Sint16 draw_43_polygons[23] = {
#include "stg29/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[19] = {
#include "stg29/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg29/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg29/terrain/draw_43.inc"
};
static Sint16 draw_42_polygons[23] = {
#include "stg29/terrain/draw_42_polygons.inc"
};
static Sint32 draw_42_vertices[19] = {
#include "stg29/terrain/draw_42_vertices.inc"
};
static NJS_CNK_MODEL draw_42_model = {
#include "stg29/terrain/draw_42_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg29/terrain/draw_42.inc"
};
static Sint16 draw_41_polygons[567] = {
#include "stg29/terrain/draw_41_polygons.inc"
};
static Sint32 draw_41_vertices[579] = {
#include "stg29/terrain/draw_41_vertices.inc"
};
static NJS_CNK_MODEL draw_41_model = {
#include "stg29/terrain/draw_41_model.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg29/terrain/draw_41.inc"
};
static Sint16 draw_40_polygons[553] = {
#include "stg29/terrain/draw_40_polygons.inc"
};
static Sint32 draw_40_vertices[563] = {
#include "stg29/terrain/draw_40_vertices.inc"
};
static NJS_CNK_MODEL draw_40_model = {
#include "stg29/terrain/draw_40_model.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg29/terrain/draw_40.inc"
};
static Sint16 draw_39_polygons[567] = {
#include "stg29/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[579] = {
#include "stg29/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg29/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg29/terrain/draw_39.inc"
};
static Sint16 draw_38_polygons[975] = {
#include "stg29/terrain/draw_38_polygons.inc"
};
static Sint32 draw_38_vertices[619] = {
#include "stg29/terrain/draw_38_vertices.inc"
};
static NJS_CNK_MODEL draw_38_model = {
#include "stg29/terrain/draw_38_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg29/terrain/draw_38.inc"
};
static Sint16 draw_37_polygons[223] = {
#include "stg29/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[159] = {
#include "stg29/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg29/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg29/terrain/draw_37.inc"
};
static Sint16 draw_36_polygons[975] = {
#include "stg29/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[619] = {
#include "stg29/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg29/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg29/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[371] = {
#include "stg29/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[339] = {
#include "stg29/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg29/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg29/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[1133] = {
#include "stg29/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[863] = {
#include "stg29/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg29/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg29/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[299] = {
#include "stg29/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[323] = {
#include "stg29/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg29/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg29/terrain/draw_33.inc"
};
static Sint16 draw_32_polygons[507] = {
#include "stg29/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[315] = {
#include "stg29/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg29/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg29/terrain/draw_32.inc"
};
static Sint16 draw_31_polygons[975] = {
#include "stg29/terrain/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[619] = {
#include "stg29/terrain/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "stg29/terrain/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg29/terrain/draw_31.inc"
};
static Sint16 draw_30_polygons[1133] = {
#include "stg29/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[863] = {
#include "stg29/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg29/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg29/terrain/draw_30.inc"
};
static Sint16 draw_29_polygons[335] = {
#include "stg29/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[311] = {
#include "stg29/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg29/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg29/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[383] = {
#include "stg29/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[255] = {
#include "stg29/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg29/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg29/terrain/draw_28.inc"
};
static Sint16 draw_27_polygons[1583] = {
#include "stg29/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[931] = {
#include "stg29/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg29/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg29/terrain/draw_27.inc"
};
static Sint16 draw_26_polygons[231] = {
#include "stg29/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[167] = {
#include "stg29/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg29/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg29/terrain/draw_26.inc"
};
static Sint16 draw_22_polygons[101] = {
#include "stg29/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[99] = {
#include "stg29/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg29/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg29/terrain/draw_25.inc"
};
static Sint16 draw_21_polygons[2253] = {
#include "stg29/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[1499] = {
#include "stg29/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg29/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg29/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[1735] = {
#include "stg29/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[1455] = {
#include "stg29/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg29/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg29/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[345] = {
#include "stg29/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[171] = {
#include "stg29/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg29/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg29/terrain/draw_19.inc"
};
static Sint16 draw_18_polygons[373] = {
#include "stg29/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[295] = {
#include "stg29/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg29/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg29/terrain/draw_18.inc"
};
static Sint16 draw_17_polygons[365] = {
#include "stg29/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[295] = {
#include "stg29/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg29/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg29/terrain/draw_17.inc"
};
static Sint16 draw_16_polygons[383] = {
#include "stg29/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[255] = {
#include "stg29/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg29/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg29/terrain/draw_16.inc"
};
static Sint16 draw_12_polygons[101] = {
#include "stg29/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[99] = {
#include "stg29/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg29/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg29/terrain/draw_15.inc"
};
static Sint16 draw_11_polygons[101] = {
#include "stg29/terrain/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[99] = {
#include "stg29/terrain/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "stg29/terrain/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg29/terrain/draw_11.inc"
};
static Sint16 draw_10_polygons[277] = {
#include "stg29/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[211] = {
#include "stg29/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg29/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg29/terrain/draw_10.inc"
};
static Sint16 draw_9_polygons[49] = {
#include "stg29/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[51] = {
#include "stg29/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg29/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg29/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[49] = {
#include "stg29/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[51] = {
#include "stg29/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg29/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg29/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[49] = {
#include "stg29/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[51] = {
#include "stg29/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg29/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg29/terrain/draw_7.inc"
};
static Sint16 draw_5_polygons[49] = {
#include "stg29/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[51] = {
#include "stg29/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg29/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg29/terrain/draw_6.inc"
};
static Sint16 draw_4_polygons[87] = {
#include "stg29/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[99] = {
#include "stg29/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg29/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg29/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[259] = {
#include "stg29/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[247] = {
#include "stg29/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg29/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg29/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[365] = {
#include "stg29/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[219] = {
#include "stg29/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg29/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg29/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[49] = {
#include "stg29/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[51] = {
#include "stg29/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg29/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg29/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[99] = {
#include "stg29/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[99] = {
#include "stg29/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg29/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg29/terrain/draw_0.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg29/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg29/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg29/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg29/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg29/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg29/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg29/terrain/draw_5.inc"
};
