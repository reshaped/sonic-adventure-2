#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg45/sand_ocean_2p.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char sand_ocean_2p_land_filenames[196] = {
#include "stg45/terrain/sand_ocean_2p_land_filenames.inc"
};

/* STG45.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between sand_ocean_2p.c and background_models.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[31];
struct land_table sand_ocean_2p_land;
static NJS_TEXNAME textures[13];
NJS_TEXLIST sand_ocean_2p_land_texlist;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[31];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[14];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[85];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[39];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_1;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[77];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[29];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[14];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[8];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[38];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[17];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[31];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[14];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[38];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[17];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static Sint16 draw_20_polygons[951];
static Sint32 draw_20_vertices[627];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_19_polygons[943];
static Sint32 draw_19_vertices[627];
static NJS_CNK_MODEL draw_19_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_17_polygons[887];
static Sint32 draw_17_vertices[587];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_18;
static Sint16 draw_15_polygons[879];
static Sint32 draw_15_vertices[587];
static NJS_CNK_MODEL draw_15_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_0_polygons[621];
static Sint32 draw_0_vertices[491];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_14;
static Sint16 draw_13_polygons[299];
static Sint32 draw_13_vertices[187];
static NJS_CNK_MODEL draw_13_model;
static NJS_CNK_OBJECT draw_13;
static Sint16 draw_12_polygons[139];
static Sint32 draw_12_vertices[111];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_12;
static Sint16 draw_7_polygons[287];
static Sint32 draw_7_vertices[287];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_11;
static Sint16 draw_6_polygons[475];
static Sint32 draw_6_vertices[435];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[553];
static Sint32 draw_5_vertices[539];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[1171];
static Sint32 draw_4_vertices[1083];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[629];
static Sint32 draw_3_vertices[623];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[707];
static Sint32 draw_2_vertices[711];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[1033];
static Sint32 draw_1_vertices[683];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static NJS_OBJECT collision_2;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_21;
static NJS_CNK_OBJECT draw_17;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[31] = {
#include "stg45/terrain/entries.inc"
};
struct land_table sand_ocean_2p_land = {
#include "stg45/terrain/sand_ocean_2p_land.inc"
};
static NJS_TEXNAME textures[13] = {
#include "stg45/terrain/textures.inc"
};
NJS_TEXLIST sand_ocean_2p_land_texlist = {
#include "stg45/terrain/sand_ocean_2p_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg45/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[31] = {
#include "stg45/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg45/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[14] = {
#include "stg45/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg45/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg45/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg45/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[85] = {
#include "stg45/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg45/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[39] = {
#include "stg45/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg45/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg45/terrain/collision_1.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg45/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[77] = {
#include "stg45/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg45/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[29] = {
#include "stg45/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg45/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg45/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg45/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[14] = {
#include "stg45/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg45/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[8] = {
#include "stg45/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg45/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg45/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg45/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[38] = {
#include "stg45/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg45/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[17] = {
#include "stg45/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg45/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg45/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg45/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[31] = {
#include "stg45/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg45/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[14] = {
#include "stg45/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg45/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg45/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg45/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[38] = {
#include "stg45/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg45/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[17] = {
#include "stg45/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg45/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg45/terrain/collision_7.inc"
};
static Sint16 draw_20_polygons[951] = {
#include "stg45/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[627] = {
#include "stg45/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg45/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg45/terrain/draw_20.inc"
};
static Sint16 draw_19_polygons[943] = {
#include "stg45/terrain/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[627] = {
#include "stg45/terrain/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "stg45/terrain/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg45/terrain/draw_19.inc"
};
static Sint16 draw_17_polygons[887] = {
#include "stg45/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[587] = {
#include "stg45/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg45/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg45/terrain/draw_18.inc"
};
static Sint16 draw_15_polygons[879] = {
#include "stg45/terrain/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[587] = {
#include "stg45/terrain/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "stg45/terrain/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg45/terrain/draw_16.inc"
};
static Sint16 draw_0_polygons[621] = {
#include "stg45/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[491] = {
#include "stg45/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg45/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg45/terrain/draw_14.inc"
};
static Sint16 draw_13_polygons[299] = {
#include "stg45/terrain/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[187] = {
#include "stg45/terrain/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "stg45/terrain/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg45/terrain/draw_13.inc"
};
static Sint16 draw_12_polygons[139] = {
#include "stg45/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[111] = {
#include "stg45/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg45/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg45/terrain/draw_12.inc"
};
static Sint16 draw_7_polygons[287] = {
#include "stg45/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[287] = {
#include "stg45/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg45/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg45/terrain/draw_11.inc"
};
static Sint16 draw_6_polygons[475] = {
#include "stg45/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[435] = {
#include "stg45/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg45/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg45/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[553] = {
#include "stg45/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[539] = {
#include "stg45/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg45/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg45/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[1171] = {
#include "stg45/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[1083] = {
#include "stg45/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg45/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg45/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[629] = {
#include "stg45/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[623] = {
#include "stg45/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg45/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg45/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[707] = {
#include "stg45/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[711] = {
#include "stg45/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg45/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg45/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[1033] = {
#include "stg45/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[683] = {
#include "stg45/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg45/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg45/terrain/draw_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg45/terrain/collision_2.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg45/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg45/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg45/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg45/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg45/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg45/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg45/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg45/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg45/terrain/draw_0.inc"
};
