#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgneut/scenery.h"
#include "collision/world.h"

#include "chaostgneut/textures/text.inc"

const char neutral_texture_filenames[128] = {
#include "chaostgneut/textures/neutral_texture_filenames.inc"
};

/* CHAOSTGNEUT.PRS's texture lists, chunk models, Basic models and land, from
 * the disc. its C lies between this file's own C and objects.c, and its D
 * lies between garden.c and objects.c */
static NJS_TEXNAME textures[14] = {
#include "chaostgneut/textures/textures.inc"
};
NJS_TEXLIST neutral_sky_texlist = {
#include "chaostgneut/textures/neutral_sky_texlist.inc"
};
static Sint16 draw_24_polygons[143] = {
#include "chaostgneut/textures/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[103] = {
#include "chaostgneut/textures/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "chaostgneut/textures/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "chaostgneut/textures/draw_24.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "chaostgneut/textures/collision_0_materials.inc"
};
static Sint16 collision_0_faces[15] = {
#include "chaostgneut/textures/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "chaostgneut/textures/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[11] = {
#include "chaostgneut/textures/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "chaostgneut/textures/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "chaostgneut/textures/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "chaostgneut/textures/collision_1_materials.inc"
};
static Sint16 collision_1_faces[27] = {
#include "chaostgneut/textures/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "chaostgneut/textures/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[17] = {
#include "chaostgneut/textures/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "chaostgneut/textures/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "chaostgneut/textures/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "chaostgneut/textures/collision_2_materials.inc"
};
static Sint16 collision_2_faces[19] = {
#include "chaostgneut/textures/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "chaostgneut/textures/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[12] = {
#include "chaostgneut/textures/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "chaostgneut/textures/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "chaostgneut/textures/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "chaostgneut/textures/collision_3_materials.inc"
};
static Sint16 collision_3_faces[347] = {
#include "chaostgneut/textures/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "chaostgneut/textures/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[135] = {
#include "chaostgneut/textures/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "chaostgneut/textures/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "chaostgneut/textures/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "chaostgneut/textures/collision_4_materials.inc"
};
static Sint16 collision_4_faces[50] = {
#include "chaostgneut/textures/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "chaostgneut/textures/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[34] = {
#include "chaostgneut/textures/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "chaostgneut/textures/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "chaostgneut/textures/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "chaostgneut/textures/collision_5_materials.inc"
};
static Sint16 collision_5_faces[107] = {
#include "chaostgneut/textures/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "chaostgneut/textures/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[60] = {
#include "chaostgneut/textures/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "chaostgneut/textures/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "chaostgneut/textures/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "chaostgneut/textures/collision_6_materials.inc"
};
static Sint16 collision_6_faces[132] = {
#include "chaostgneut/textures/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "chaostgneut/textures/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[86] = {
#include "chaostgneut/textures/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "chaostgneut/textures/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "chaostgneut/textures/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "chaostgneut/textures/collision_7_materials.inc"
};
static Sint16 collision_7_faces[37] = {
#include "chaostgneut/textures/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "chaostgneut/textures/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[25] = {
#include "chaostgneut/textures/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "chaostgneut/textures/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "chaostgneut/textures/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "chaostgneut/textures/collision_8_materials.inc"
};
static Sint16 collision_8_faces[106] = {
#include "chaostgneut/textures/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "chaostgneut/textures/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[42] = {
#include "chaostgneut/textures/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "chaostgneut/textures/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "chaostgneut/textures/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "chaostgneut/textures/collision_9_materials.inc"
};
static Sint16 collision_9_faces[18] = {
#include "chaostgneut/textures/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "chaostgneut/textures/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[11] = {
#include "chaostgneut/textures/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "chaostgneut/textures/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "chaostgneut/textures/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "chaostgneut/textures/collision_10_materials.inc"
};
static Sint16 collision_10_faces[39] = {
#include "chaostgneut/textures/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "chaostgneut/textures/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[18] = {
#include "chaostgneut/textures/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "chaostgneut/textures/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "chaostgneut/textures/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "chaostgneut/textures/collision_11_materials.inc"
};
static Sint16 collision_11_faces[17] = {
#include "chaostgneut/textures/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "chaostgneut/textures/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[9] = {
#include "chaostgneut/textures/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "chaostgneut/textures/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "chaostgneut/textures/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "chaostgneut/textures/collision_12_materials.inc"
};
static Sint16 collision_12_faces[20] = {
#include "chaostgneut/textures/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "chaostgneut/textures/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[10] = {
#include "chaostgneut/textures/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "chaostgneut/textures/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "chaostgneut/textures/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "chaostgneut/textures/collision_13_materials.inc"
};
static Sint16 collision_13_faces[141] = {
#include "chaostgneut/textures/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "chaostgneut/textures/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[74] = {
#include "chaostgneut/textures/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "chaostgneut/textures/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "chaostgneut/textures/collision_13.inc"
};
static Sint16 draw_23_polygons[673] = {
#include "chaostgneut/textures/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[411] = {
#include "chaostgneut/textures/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "chaostgneut/textures/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "chaostgneut/textures/draw_23.inc"
};
static Sint16 draw_22_polygons[199] = {
#include "chaostgneut/textures/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[135] = {
#include "chaostgneut/textures/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "chaostgneut/textures/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "chaostgneut/textures/draw_22.inc"
};
static Sint16 draw_21_polygons[555] = {
#include "chaostgneut/textures/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[331] = {
#include "chaostgneut/textures/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "chaostgneut/textures/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "chaostgneut/textures/draw_21.inc"
};
static Sint16 draw_20_polygons[407] = {
#include "chaostgneut/textures/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[307] = {
#include "chaostgneut/textures/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "chaostgneut/textures/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "chaostgneut/textures/draw_20.inc"
};
static Sint16 draw_19_polygons[229] = {
#include "chaostgneut/textures/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[159] = {
#include "chaostgneut/textures/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "chaostgneut/textures/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "chaostgneut/textures/draw_19.inc"
};
static Sint16 neutral_lower_falls_polygons[177] = {
#include "chaostgneut/textures/neutral_lower_falls_polygons.inc"
};
static Sint32 neutral_lower_falls_vertices[119] = {
#include "chaostgneut/textures/neutral_lower_falls_vertices.inc"
};
static NJS_CNK_MODEL neutral_lower_falls_model = {
#include "chaostgneut/textures/neutral_lower_falls_model.inc"
};
NJS_CNK_OBJECT neutral_lower_falls = {
#include "chaostgneut/textures/neutral_lower_falls.inc"
};
static Sint16 neutral_upper_falls_polygons[303] = {
#include "chaostgneut/textures/neutral_upper_falls_polygons.inc"
};
static Sint32 neutral_upper_falls_vertices[219] = {
#include "chaostgneut/textures/neutral_upper_falls_vertices.inc"
};
static NJS_CNK_MODEL neutral_upper_falls_model = {
#include "chaostgneut/textures/neutral_upper_falls_model.inc"
};
NJS_CNK_OBJECT neutral_upper_falls = {
#include "chaostgneut/textures/neutral_upper_falls.inc"
};
static Sint16 draw_18_polygons[447] = {
#include "chaostgneut/textures/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[267] = {
#include "chaostgneut/textures/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "chaostgneut/textures/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "chaostgneut/textures/draw_18.inc"
};
static Sint16 draw_17_polygons[503] = {
#include "chaostgneut/textures/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[287] = {
#include "chaostgneut/textures/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "chaostgneut/textures/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "chaostgneut/textures/draw_17.inc"
};
static Sint16 draw_16_polygons[121] = {
#include "chaostgneut/textures/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[95] = {
#include "chaostgneut/textures/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "chaostgneut/textures/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "chaostgneut/textures/draw_16.inc"
};
static Sint16 draw_15_polygons[215] = {
#include "chaostgneut/textures/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[139] = {
#include "chaostgneut/textures/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "chaostgneut/textures/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "chaostgneut/textures/draw_15.inc"
};
static Sint16 draw_14_polygons[267] = {
#include "chaostgneut/textures/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[211] = {
#include "chaostgneut/textures/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "chaostgneut/textures/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "chaostgneut/textures/draw_14.inc"
};
static Sint16 draw_13_polygons[605] = {
#include "chaostgneut/textures/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[387] = {
#include "chaostgneut/textures/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "chaostgneut/textures/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "chaostgneut/textures/draw_13.inc"
};
static Sint16 draw_12_polygons[143] = {
#include "chaostgneut/textures/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[103] = {
#include "chaostgneut/textures/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "chaostgneut/textures/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "chaostgneut/textures/draw_12.inc"
};
static Sint16 draw_11_polygons[77] = {
#include "chaostgneut/textures/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[83] = {
#include "chaostgneut/textures/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "chaostgneut/textures/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "chaostgneut/textures/draw_11.inc"
};
static Sint16 draw_10_polygons[143] = {
#include "chaostgneut/textures/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[103] = {
#include "chaostgneut/textures/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "chaostgneut/textures/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "chaostgneut/textures/draw_10.inc"
};
static Sint16 draw_9_polygons[143] = {
#include "chaostgneut/textures/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[103] = {
#include "chaostgneut/textures/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "chaostgneut/textures/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "chaostgneut/textures/draw_9.inc"
};
static Sint16 draw_8_polygons[143] = {
#include "chaostgneut/textures/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[103] = {
#include "chaostgneut/textures/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "chaostgneut/textures/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "chaostgneut/textures/draw_8.inc"
};
static Sint16 draw_7_polygons[143] = {
#include "chaostgneut/textures/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[103] = {
#include "chaostgneut/textures/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "chaostgneut/textures/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "chaostgneut/textures/draw_7.inc"
};
static Sint16 draw_6_polygons[77] = {
#include "chaostgneut/textures/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[83] = {
#include "chaostgneut/textures/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "chaostgneut/textures/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "chaostgneut/textures/draw_6.inc"
};
static Sint16 draw_5_polygons[77] = {
#include "chaostgneut/textures/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[83] = {
#include "chaostgneut/textures/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "chaostgneut/textures/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "chaostgneut/textures/draw_5.inc"
};
static Sint16 draw_4_polygons[77] = {
#include "chaostgneut/textures/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[83] = {
#include "chaostgneut/textures/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "chaostgneut/textures/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "chaostgneut/textures/draw_4.inc"
};
static Sint16 draw_3_polygons[77] = {
#include "chaostgneut/textures/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[83] = {
#include "chaostgneut/textures/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "chaostgneut/textures/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "chaostgneut/textures/draw_3.inc"
};
static Sint16 draw_2_polygons[77] = {
#include "chaostgneut/textures/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[83] = {
#include "chaostgneut/textures/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "chaostgneut/textures/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "chaostgneut/textures/draw_2.inc"
};
static Sint16 draw_1_polygons[143] = {
#include "chaostgneut/textures/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[103] = {
#include "chaostgneut/textures/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "chaostgneut/textures/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "chaostgneut/textures/draw_1.inc"
};
static Sint16 neutral_inner_sky_polygons[165] = {
#include "chaostgneut/textures/neutral_inner_sky_polygons.inc"
};
static Sint32 neutral_inner_sky_vertices[111] = {
#include "chaostgneut/textures/neutral_inner_sky_vertices.inc"
};
static NJS_CNK_MODEL neutral_inner_sky_model = {
#include "chaostgneut/textures/neutral_inner_sky_model.inc"
};
NJS_CNK_OBJECT neutral_inner_sky = {
#include "chaostgneut/textures/neutral_inner_sky.inc"
};
static Sint16 neutral_outer_sky_polygons[227] = {
#include "chaostgneut/textures/neutral_outer_sky_polygons.inc"
};
static Sint32 neutral_outer_sky_vertices[139] = {
#include "chaostgneut/textures/neutral_outer_sky_vertices.inc"
};
static NJS_CNK_MODEL neutral_outer_sky_model = {
#include "chaostgneut/textures/neutral_outer_sky_model.inc"
};
NJS_CNK_OBJECT neutral_outer_sky = {
#include "chaostgneut/textures/neutral_outer_sky.inc"
};
static Sint16 draw_0_polygons[127] = {
#include "chaostgneut/textures/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[75] = {
#include "chaostgneut/textures/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "chaostgneut/textures/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "chaostgneut/textures/draw_0.inc"
};
static Sint16 neutral_pond_polygons[249] = {
#include "chaostgneut/textures/neutral_pond_polygons.inc"
};
static Sint32 neutral_pond_vertices[155] = {
#include "chaostgneut/textures/neutral_pond_vertices.inc"
};
static NJS_CNK_MODEL neutral_pond_model = {
#include "chaostgneut/textures/neutral_pond_model.inc"
};
NJS_CNK_OBJECT neutral_pond = {
#include "chaostgneut/textures/neutral_pond.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "chaostgneut/textures/collision_14_materials.inc"
};
static Sint16 collision_14_faces[78] = {
#include "chaostgneut/textures/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "chaostgneut/textures/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[35] = {
#include "chaostgneut/textures/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "chaostgneut/textures/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "chaostgneut/textures/collision_14.inc"
};
static struct land_entry entries[40] = {
#include "chaostgneut/textures/entries.inc"
};
struct land_table neutral_land = {
#include "chaostgneut/textures/neutral_land.inc"
};
