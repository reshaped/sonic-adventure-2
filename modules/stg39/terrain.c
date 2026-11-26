#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg39/mission_street_2p.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char mission_street_2p_land_filenames[337] = {
#include "stg39/terrain/mission_street_2p_land_filenames.inc"
};

/* STG39.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between mission_street_2p.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[65];
struct land_table mission_street_2p_land;
static NJS_TEXNAME textures[25];
NJS_TEXLIST mission_street_2p_land_texlist;
static NJS_CNK_OBJECT empty_node_0;
static Sint16 draw_37_polygons[339];
static Sint32 draw_37_vertices[251];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT empty_node_1;
static NJS_CNK_OBJECT empty_node_2;
static Sint16 draw_34_polygons[759];
static Sint32 draw_34_vertices[495];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_33_polygons[431];
static Sint32 draw_33_vertices[515];
static NJS_CNK_MODEL draw_33_model;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT empty_node_3;
static Sint16 draw_32_polygons[339];
static Sint32 draw_32_vertices[255];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT empty_node_4;
static Sint16 draw_30_polygons[55];
static Sint32 draw_30_vertices[47];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_29_polygons[1595];
static Sint32 draw_29_vertices[1291];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_25_polygons[689];
static Sint32 draw_25_vertices[519];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_22_polygons[741];
static Sint32 draw_22_vertices[543];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[637];
static Sint32 draw_23_vertices[543];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_16_polygons[103];
static Sint32 draw_16_vertices[63];
static NJS_CNK_MODEL draw_16_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_15_polygons[431];
static Sint32 draw_15_vertices[515];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_15;
static Sint16 draw_14_polygons[759];
static Sint32 draw_14_vertices[495];
static NJS_CNK_MODEL draw_14_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_12_polygons[435];
static Sint32 draw_12_vertices[515];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_10_polygons[759];
static Sint32 draw_10_vertices[495];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_6_polygons[339];
static Sint32 draw_6_vertices[347];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_0_polygons[519];
static Sint32 draw_0_vertices[451];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_1_polygons[339];
static Sint32 draw_1_vertices[347];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_4;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[10];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[8];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[14];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[9];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[14];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[9];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[10];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[8];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[17];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[12];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[14];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[12];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[14];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[12];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[17];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[12];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[15];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[12];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[47];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[30];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[28];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[21];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[136];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[60];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[63];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[23];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[28];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[21];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_25;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_25;

static struct land_entry entries[65] = {
#include "stg39/terrain/entries.inc"
};
struct land_table mission_street_2p_land = {
#include "stg39/terrain/mission_street_2p_land.inc"
};
static NJS_TEXNAME textures[25] = {
#include "stg39/terrain/textures.inc"
};
NJS_TEXLIST mission_street_2p_land_texlist = {
#include "stg39/terrain/mission_street_2p_land_texlist.inc"
};
static NJS_CNK_OBJECT empty_node_0 = {
#include "stg39/terrain/empty_node_0.inc"
};
static Sint16 draw_37_polygons[339] = {
#include "stg39/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[251] = {
#include "stg39/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg39/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg39/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT empty_node_1 = {
#include "stg39/terrain/empty_node_1.inc"
};
static NJS_CNK_OBJECT empty_node_2 = {
#include "stg39/terrain/empty_node_2.inc"
};
static Sint16 draw_34_polygons[759] = {
#include "stg39/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[495] = {
#include "stg39/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg39/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg39/terrain/draw_34.inc"
};
static Sint16 draw_33_polygons[431] = {
#include "stg39/terrain/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[515] = {
#include "stg39/terrain/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "stg39/terrain/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg39/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT empty_node_3 = {
#include "stg39/terrain/empty_node_3.inc"
};
static Sint16 draw_32_polygons[339] = {
#include "stg39/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[255] = {
#include "stg39/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg39/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg39/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT empty_node_4 = {
#include "stg39/terrain/empty_node_4.inc"
};
static Sint16 draw_30_polygons[55] = {
#include "stg39/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[47] = {
#include "stg39/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg39/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg39/terrain/draw_31.inc"
};
static Sint16 draw_29_polygons[1595] = {
#include "stg39/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[1291] = {
#include "stg39/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg39/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg39/terrain/draw_29.inc"
};
static Sint16 draw_25_polygons[689] = {
#include "stg39/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[519] = {
#include "stg39/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg39/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg39/terrain/draw_26.inc"
};
static Sint16 draw_22_polygons[741] = {
#include "stg39/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[543] = {
#include "stg39/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg39/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg39/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[637] = {
#include "stg39/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[543] = {
#include "stg39/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg39/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg39/terrain/draw_23.inc"
};
static Sint16 draw_16_polygons[103] = {
#include "stg39/terrain/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[63] = {
#include "stg39/terrain/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "stg39/terrain/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg39/terrain/draw_21.inc"
};
static Sint16 draw_15_polygons[431] = {
#include "stg39/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[515] = {
#include "stg39/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg39/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg39/terrain/draw_15.inc"
};
static Sint16 draw_14_polygons[759] = {
#include "stg39/terrain/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[495] = {
#include "stg39/terrain/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "stg39/terrain/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg39/terrain/draw_14.inc"
};
static Sint16 draw_12_polygons[435] = {
#include "stg39/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[515] = {
#include "stg39/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg39/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg39/terrain/draw_13.inc"
};
static Sint16 draw_10_polygons[759] = {
#include "stg39/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[495] = {
#include "stg39/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg39/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg39/terrain/draw_11.inc"
};
static Sint16 draw_6_polygons[339] = {
#include "stg39/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[347] = {
#include "stg39/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg39/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg39/terrain/draw_9.inc"
};
static Sint16 draw_0_polygons[519] = {
#include "stg39/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[451] = {
#include "stg39/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg39/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg39/terrain/draw_5.inc"
};
static Sint16 draw_1_polygons[339] = {
#include "stg39/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[347] = {
#include "stg39/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg39/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg39/terrain/draw_4.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg39/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[10] = {
#include "stg39/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg39/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[8] = {
#include "stg39/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg39/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg39/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg39/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[14] = {
#include "stg39/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg39/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[9] = {
#include "stg39/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg39/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg39/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg39/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[14] = {
#include "stg39/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg39/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[9] = {
#include "stg39/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg39/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg39/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg39/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[10] = {
#include "stg39/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg39/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[8] = {
#include "stg39/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg39/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg39/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg39/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[17] = {
#include "stg39/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg39/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[12] = {
#include "stg39/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg39/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg39/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg39/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[14] = {
#include "stg39/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg39/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[12] = {
#include "stg39/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg39/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg39/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg39/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[14] = {
#include "stg39/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg39/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[12] = {
#include "stg39/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg39/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg39/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg39/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[17] = {
#include "stg39/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg39/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[12] = {
#include "stg39/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg39/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg39/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg39/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[15] = {
#include "stg39/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg39/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[12] = {
#include "stg39/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg39/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg39/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg39/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[47] = {
#include "stg39/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg39/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[30] = {
#include "stg39/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg39/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg39/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg39/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[28] = {
#include "stg39/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg39/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[21] = {
#include "stg39/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg39/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg39/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg39/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[136] = {
#include "stg39/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg39/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[60] = {
#include "stg39/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg39/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg39/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg39/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[63] = {
#include "stg39/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg39/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[23] = {
#include "stg39/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg39/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg39/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg39/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[28] = {
#include "stg39/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg39/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[21] = {
#include "stg39/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg39/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg39/terrain/collision_26.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg39/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg39/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg39/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg39/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg39/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg39/terrain/draw_25.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg39/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg39/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg39/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg39/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg39/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg39/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg39/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg39/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg39/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg39/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg39/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg39/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg39/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg39/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg39/terrain/draw_0.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg39/terrain/collision_2.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg39/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg39/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg39/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg39/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg39/terrain/collision_11.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg39/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg39/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg39/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg39/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg39/terrain/collision_16.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg39/terrain/collision_20.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg39/terrain/collision_25.inc"
};
