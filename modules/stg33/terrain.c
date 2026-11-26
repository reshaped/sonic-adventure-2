#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg33/knuckles_rouge.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char knuckles_rouge_land_filenames[230] = {
#include "stg33/terrain/knuckles_rouge_land_filenames.inc"
};

/* STG33.PRS's land, texture lists, Basic models and chunk models, from the
 * disc. its D lies between knuckles_rouge.c and object_archives.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[74];
struct land_table knuckles_rouge_land;
static NJS_TEXNAME textures[18];
static NJS_TEXLIST terrain_textures;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[21];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[12];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[27];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[16];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[14];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[9];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static Sint16 draw_56_polygons[347];
static Sint32 draw_56_vertices[195];
static NJS_CNK_MODEL draw_56_model;
static NJS_CNK_OBJECT draw_56;
static Sint16 draw_55_polygons[205];
static Sint32 draw_55_vertices[195];
static NJS_CNK_MODEL draw_55_model;
static NJS_CNK_OBJECT draw_55;
static Sint16 draw_51_polygons[55];
static Sint32 draw_51_vertices[43];
static NJS_CNK_MODEL draw_51_model;
static NJS_CNK_OBJECT draw_54;
static Sint16 draw_47_polygons[103];
static Sint32 draw_47_vertices[83];
static NJS_CNK_MODEL draw_47_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_43_polygons[157];
static Sint32 draw_43_vertices[123];
static NJS_CNK_MODEL draw_43_model;
static NJS_CNK_OBJECT draw_46;
static Sint16 draw_39_polygons[55];
static Sint32 draw_39_vertices[43];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_42;
static Sint16 draw_27_polygons[73];
static Sint32 draw_27_vertices[75];
static NJS_CNK_MODEL draw_27_model;
static NJS_CNK_OBJECT draw_38;
static Sint16 draw_26_polygons[121];
static Sint32 draw_26_vertices[67];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_26;
static Sint16 draw_24_polygons[121];
static Sint32 draw_24_vertices[67];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_12_polygons[101];
static Sint32 draw_12_vertices[99];
static NJS_CNK_MODEL draw_12_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_0_polygons[55];
static Sint32 draw_0_vertices[43];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_11;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[14];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[9];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_2;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_5;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_7;
static NJS_OBJECT collision_8;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_10;
static NJS_OBJECT collision_11;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_27;
static NJS_CNK_OBJECT draw_24;
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
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_7;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;

static struct land_entry entries[74] = {
#include "stg33/terrain/entries.inc"
};
struct land_table knuckles_rouge_land = {
#include "stg33/terrain/knuckles_rouge_land.inc"
};
static NJS_TEXNAME textures[18] = {
#include "stg33/terrain/textures.inc"
};
static NJS_TEXLIST terrain_textures = {
#include "stg33/terrain/terrain_textures.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg33/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[21] = {
#include "stg33/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg33/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[12] = {
#include "stg33/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg33/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg33/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg33/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[27] = {
#include "stg33/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg33/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[16] = {
#include "stg33/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg33/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg33/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg33/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[14] = {
#include "stg33/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg33/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[9] = {
#include "stg33/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg33/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg33/terrain/collision_12.inc"
};
static Sint16 draw_56_polygons[347] = {
#include "stg33/terrain/draw_56_polygons.inc"
};
static Sint32 draw_56_vertices[195] = {
#include "stg33/terrain/draw_56_vertices.inc"
};
static NJS_CNK_MODEL draw_56_model = {
#include "stg33/terrain/draw_56_model.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg33/terrain/draw_56.inc"
};
static Sint16 draw_55_polygons[205] = {
#include "stg33/terrain/draw_55_polygons.inc"
};
static Sint32 draw_55_vertices[195] = {
#include "stg33/terrain/draw_55_vertices.inc"
};
static NJS_CNK_MODEL draw_55_model = {
#include "stg33/terrain/draw_55_model.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg33/terrain/draw_55.inc"
};
static Sint16 draw_51_polygons[55] = {
#include "stg33/terrain/draw_51_polygons.inc"
};
static Sint32 draw_51_vertices[43] = {
#include "stg33/terrain/draw_51_vertices.inc"
};
static NJS_CNK_MODEL draw_51_model = {
#include "stg33/terrain/draw_51_model.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg33/terrain/draw_54.inc"
};
static Sint16 draw_47_polygons[103] = {
#include "stg33/terrain/draw_47_polygons.inc"
};
static Sint32 draw_47_vertices[83] = {
#include "stg33/terrain/draw_47_vertices.inc"
};
static NJS_CNK_MODEL draw_47_model = {
#include "stg33/terrain/draw_47_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg33/terrain/draw_50.inc"
};
static Sint16 draw_43_polygons[157] = {
#include "stg33/terrain/draw_43_polygons.inc"
};
static Sint32 draw_43_vertices[123] = {
#include "stg33/terrain/draw_43_vertices.inc"
};
static NJS_CNK_MODEL draw_43_model = {
#include "stg33/terrain/draw_43_model.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg33/terrain/draw_46.inc"
};
static Sint16 draw_39_polygons[55] = {
#include "stg33/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[43] = {
#include "stg33/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg33/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg33/terrain/draw_42.inc"
};
static Sint16 draw_27_polygons[73] = {
#include "stg33/terrain/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[75] = {
#include "stg33/terrain/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "stg33/terrain/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg33/terrain/draw_38.inc"
};
static Sint16 draw_26_polygons[121] = {
#include "stg33/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[67] = {
#include "stg33/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg33/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg33/terrain/draw_26.inc"
};
static Sint16 draw_24_polygons[121] = {
#include "stg33/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[67] = {
#include "stg33/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg33/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg33/terrain/draw_25.inc"
};
static Sint16 draw_12_polygons[101] = {
#include "stg33/terrain/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[99] = {
#include "stg33/terrain/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "stg33/terrain/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg33/terrain/draw_23.inc"
};
static Sint16 draw_0_polygons[55] = {
#include "stg33/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[43] = {
#include "stg33/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg33/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg33/terrain/draw_11.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg33/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[14] = {
#include "stg33/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg33/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[9] = {
#include "stg33/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg33/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg33/terrain/collision_16.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg33/terrain/collision_1.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg33/terrain/collision_2.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg33/terrain/collision_3.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg33/terrain/collision_5.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg33/terrain/collision_6.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg33/terrain/collision_7.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg33/terrain/collision_8.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg33/terrain/collision_9.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg33/terrain/collision_10.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg33/terrain/collision_11.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg33/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg33/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg33/terrain/collision_15.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg33/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg33/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg33/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg33/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg33/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg33/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg33/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg33/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg33/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg33/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg33/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg33/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg33/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg33/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg33/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg33/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg33/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg33/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg33/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg33/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg33/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg33/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg33/terrain/draw_27.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg33/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg33/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg33/terrain/draw_21.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg33/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg33/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg33/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg33/terrain/draw_17.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg33/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg33/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg33/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg33/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg33/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg33/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg33/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg33/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg33/terrain/draw_7.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg33/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg33/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg33/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg33/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg33/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg33/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg33/terrain/draw_0.inc"
};
