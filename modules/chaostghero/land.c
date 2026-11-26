/* CHAOSTGHERO.PRS's texture lists, Basic models, chunk models, land and
 * texture names, from the disc. its C lies between garden_prop_editor.c and
 * objects.c, and its D lies between prop_models.c and objects.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostghero/land.h"
#include "collision/world.h"

#include "chaostghero/land/text.inc"

static NJS_TEXNAME textures[38] = {
#include "chaostghero/land/textures.inc"
};
NJS_TEXLIST hero_land_texlist = {
#include "chaostghero/land/hero_land_texlist.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "chaostghero/land/collision_0_materials.inc"
};
static Sint16 collision_0_faces[30] = {
#include "chaostghero/land/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "chaostghero/land/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[15] = {
#include "chaostghero/land/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "chaostghero/land/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "chaostghero/land/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "chaostghero/land/collision_1_materials.inc"
};
static Sint16 collision_1_faces[37] = {
#include "chaostghero/land/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "chaostghero/land/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[17] = {
#include "chaostghero/land/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "chaostghero/land/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "chaostghero/land/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "chaostghero/land/collision_2_materials.inc"
};
static Sint16 collision_2_faces[36] = {
#include "chaostghero/land/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "chaostghero/land/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[17] = {
#include "chaostghero/land/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "chaostghero/land/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "chaostghero/land/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "chaostghero/land/collision_3_materials.inc"
};
static Sint16 collision_3_faces[25] = {
#include "chaostghero/land/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "chaostghero/land/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[13] = {
#include "chaostghero/land/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "chaostghero/land/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "chaostghero/land/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "chaostghero/land/collision_4_materials.inc"
};
static Sint16 collision_4_faces[28] = {
#include "chaostghero/land/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "chaostghero/land/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[13] = {
#include "chaostghero/land/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "chaostghero/land/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "chaostghero/land/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "chaostghero/land/collision_5_materials.inc"
};
static Sint16 collision_5_faces[25] = {
#include "chaostghero/land/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "chaostghero/land/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[14] = {
#include "chaostghero/land/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "chaostghero/land/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "chaostghero/land/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "chaostghero/land/collision_6_materials.inc"
};
static Sint16 collision_6_faces[32] = {
#include "chaostghero/land/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "chaostghero/land/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[12] = {
#include "chaostghero/land/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "chaostghero/land/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "chaostghero/land/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "chaostghero/land/collision_7_materials.inc"
};
static Sint16 collision_7_faces[29] = {
#include "chaostghero/land/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "chaostghero/land/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[16] = {
#include "chaostghero/land/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "chaostghero/land/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "chaostghero/land/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "chaostghero/land/collision_8_materials.inc"
};
static Sint16 collision_8_faces[56] = {
#include "chaostghero/land/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "chaostghero/land/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[25] = {
#include "chaostghero/land/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "chaostghero/land/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "chaostghero/land/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "chaostghero/land/collision_9_materials.inc"
};
static Sint16 collision_9_faces[38] = {
#include "chaostghero/land/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "chaostghero/land/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[28] = {
#include "chaostghero/land/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "chaostghero/land/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "chaostghero/land/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "chaostghero/land/collision_10_materials.inc"
};
static Sint16 collision_10_faces[25] = {
#include "chaostghero/land/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "chaostghero/land/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[21] = {
#include "chaostghero/land/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "chaostghero/land/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "chaostghero/land/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "chaostghero/land/collision_11_materials.inc"
};
static Sint16 collision_11_faces[24] = {
#include "chaostghero/land/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "chaostghero/land/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[19] = {
#include "chaostghero/land/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "chaostghero/land/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "chaostghero/land/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "chaostghero/land/collision_12_materials.inc"
};
static Sint16 collision_12_faces[25] = {
#include "chaostghero/land/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "chaostghero/land/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[18] = {
#include "chaostghero/land/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "chaostghero/land/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "chaostghero/land/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "chaostghero/land/collision_13_materials.inc"
};
static Sint16 collision_13_faces[48] = {
#include "chaostghero/land/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "chaostghero/land/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[32] = {
#include "chaostghero/land/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "chaostghero/land/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "chaostghero/land/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "chaostghero/land/collision_14_materials.inc"
};
static Sint16 collision_14_faces[80] = {
#include "chaostghero/land/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "chaostghero/land/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[53] = {
#include "chaostghero/land/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "chaostghero/land/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "chaostghero/land/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "chaostghero/land/collision_15_materials.inc"
};
static Sint16 collision_15_faces[5] = {
#include "chaostghero/land/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "chaostghero/land/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[4] = {
#include "chaostghero/land/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "chaostghero/land/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "chaostghero/land/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "chaostghero/land/collision_16_materials.inc"
};
static Sint16 collision_16_faces[10] = {
#include "chaostghero/land/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "chaostghero/land/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[8] = {
#include "chaostghero/land/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "chaostghero/land/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "chaostghero/land/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "chaostghero/land/collision_17_materials.inc"
};
static Sint16 collision_17_faces[10] = {
#include "chaostghero/land/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "chaostghero/land/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[8] = {
#include "chaostghero/land/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "chaostghero/land/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "chaostghero/land/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "chaostghero/land/collision_18_materials.inc"
};
static Sint16 collision_18_faces[36] = {
#include "chaostghero/land/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "chaostghero/land/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[24] = {
#include "chaostghero/land/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "chaostghero/land/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "chaostghero/land/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "chaostghero/land/collision_19_materials.inc"
};
static Sint16 collision_19_faces[73] = {
#include "chaostghero/land/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "chaostghero/land/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[40] = {
#include "chaostghero/land/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "chaostghero/land/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "chaostghero/land/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "chaostghero/land/collision_20_materials.inc"
};
static Sint16 collision_20_faces[15] = {
#include "chaostghero/land/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "chaostghero/land/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[14] = {
#include "chaostghero/land/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "chaostghero/land/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "chaostghero/land/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "chaostghero/land/collision_21_materials.inc"
};
static Sint16 collision_21_faces[12] = {
#include "chaostghero/land/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "chaostghero/land/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[8] = {
#include "chaostghero/land/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "chaostghero/land/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "chaostghero/land/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "chaostghero/land/collision_22_materials.inc"
};
static Sint16 collision_22_faces[22] = {
#include "chaostghero/land/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "chaostghero/land/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[15] = {
#include "chaostghero/land/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "chaostghero/land/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "chaostghero/land/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "chaostghero/land/collision_23_materials.inc"
};
static Sint16 collision_23_faces[9] = {
#include "chaostghero/land/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "chaostghero/land/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[8] = {
#include "chaostghero/land/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "chaostghero/land/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "chaostghero/land/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "chaostghero/land/collision_24_materials.inc"
};
static Sint16 collision_24_faces[17] = {
#include "chaostghero/land/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "chaostghero/land/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[13] = {
#include "chaostghero/land/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "chaostghero/land/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "chaostghero/land/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "chaostghero/land/collision_25_materials.inc"
};
static Sint16 collision_25_faces[17] = {
#include "chaostghero/land/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "chaostghero/land/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[10] = {
#include "chaostghero/land/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "chaostghero/land/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "chaostghero/land/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "chaostghero/land/collision_26_materials.inc"
};
static Sint16 collision_26_faces[41] = {
#include "chaostghero/land/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "chaostghero/land/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[30] = {
#include "chaostghero/land/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "chaostghero/land/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "chaostghero/land/collision_26.inc"
};
static Sint16 draw_32_polygons[349] = {
#include "chaostghero/land/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[375] = {
#include "chaostghero/land/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "chaostghero/land/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "chaostghero/land/draw_32.inc"
};
static Sint16 draw_31_polygons[141] = {
#include "chaostghero/land/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[67] = {
#include "chaostghero/land/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "chaostghero/land/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "chaostghero/land/draw_31.inc"
};
static Sint16 draw_30_polygons[105] = {
#include "chaostghero/land/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[51] = {
#include "chaostghero/land/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "chaostghero/land/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "chaostghero/land/draw_30.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "chaostghero/land/collision_27_materials.inc"
};
static Sint16 collision_27_faces[19] = {
#include "chaostghero/land/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "chaostghero/land/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[12] = {
#include "chaostghero/land/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "chaostghero/land/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "chaostghero/land/collision_27.inc"
};
static Sint16 draw_29_polygons[59] = {
#include "chaostghero/land/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[141] = {
#include "chaostghero/land/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "chaostghero/land/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "chaostghero/land/draw_29.inc"
};
static Sint16 draw_28_polygons[229] = {
#include "chaostghero/land/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[147] = {
#include "chaostghero/land/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "chaostghero/land/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "chaostghero/land/draw_28.inc"
};
static Sint16 draw_27_polygons[353] = {
#include "chaostghero/land/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[227] = {
#include "chaostghero/land/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "chaostghero/land/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "chaostghero/land/draw_27.inc"
};
static Sint16 draw_26_polygons[547] = {
#include "chaostghero/land/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[479] = {
#include "chaostghero/land/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "chaostghero/land/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "chaostghero/land/draw_26.inc"
};
static Sint16 draw_25_polygons[111] = {
#include "chaostghero/land/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[75] = {
#include "chaostghero/land/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "chaostghero/land/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "chaostghero/land/draw_25.inc"
};
static Sint16 draw_24_polygons[25] = {
#include "chaostghero/land/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[19] = {
#include "chaostghero/land/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "chaostghero/land/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "chaostghero/land/draw_24.inc"
};
static Sint16 draw_23_polygons[107] = {
#include "chaostghero/land/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[75] = {
#include "chaostghero/land/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "chaostghero/land/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "chaostghero/land/draw_23.inc"
};
static Sint16 draw_22_polygons[67] = {
#include "chaostghero/land/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[43] = {
#include "chaostghero/land/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "chaostghero/land/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "chaostghero/land/draw_22.inc"
};
static Sint16 draw_21_polygons[193] = {
#include "chaostghero/land/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[119] = {
#include "chaostghero/land/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "chaostghero/land/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "chaostghero/land/draw_21.inc"
};
static Sint16 draw_20_polygons[133] = {
#include "chaostghero/land/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[83] = {
#include "chaostghero/land/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "chaostghero/land/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "chaostghero/land/draw_20.inc"
};
static Sint16 draw_19_polygons[235] = {
#include "chaostghero/land/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[223] = {
#include "chaostghero/land/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "chaostghero/land/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "chaostghero/land/draw_19.inc"
};
static Sint16 draw_18_polygons[197] = {
#include "chaostghero/land/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[135] = {
#include "chaostghero/land/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "chaostghero/land/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "chaostghero/land/draw_18.inc"
};
static Sint16 draw_17_polygons[595] = {
#include "chaostghero/land/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[483] = {
#include "chaostghero/land/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "chaostghero/land/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "chaostghero/land/draw_17.inc"
};
static Sint16 draw_16_polygons[207] = {
#include "chaostghero/land/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[119] = {
#include "chaostghero/land/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "chaostghero/land/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "chaostghero/land/draw_16.inc"
};
static Sint16 draw_15_polygons[119] = {
#include "chaostghero/land/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[71] = {
#include "chaostghero/land/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "chaostghero/land/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "chaostghero/land/draw_15.inc"
};
static Sint16 draw_14_polygons[175] = {
#include "chaostghero/land/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[99] = {
#include "chaostghero/land/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "chaostghero/land/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "chaostghero/land/draw_14.inc"
};
static Sint16 draw_13_polygons[175] = {
#include "chaostghero/land/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[99] = {
#include "chaostghero/land/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "chaostghero/land/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "chaostghero/land/draw_13.inc"
};
static Sint16 draw_12_polygons[201] = {
#include "chaostghero/land/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[127] = {
#include "chaostghero/land/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "chaostghero/land/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "chaostghero/land/draw_12.inc"
};
static Sint16 draw_11_polygons[197] = {
#include "chaostghero/land/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[115] = {
#include "chaostghero/land/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "chaostghero/land/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "chaostghero/land/draw_11.inc"
};
static Sint16 draw_10_polygons[193] = {
#include "chaostghero/land/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[111] = {
#include "chaostghero/land/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "chaostghero/land/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "chaostghero/land/draw_10.inc"
};
static Sint16 draw_9_polygons[323] = {
#include "chaostghero/land/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[187] = {
#include "chaostghero/land/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "chaostghero/land/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "chaostghero/land/draw_9.inc"
};
static Sint16 draw_8_polygons[177] = {
#include "chaostghero/land/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[123] = {
#include "chaostghero/land/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "chaostghero/land/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "chaostghero/land/draw_8.inc"
};
static Sint16 draw_7_polygons[173] = {
#include "chaostghero/land/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[119] = {
#include "chaostghero/land/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "chaostghero/land/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "chaostghero/land/draw_7.inc"
};
static Sint16 hero_fountain_base_polygons[121] = {
#include "chaostghero/land/hero_fountain_base_polygons.inc"
};
static Sint32 hero_fountain_base_vertices[75] = {
#include "chaostghero/land/hero_fountain_base_vertices.inc"
};
static NJS_CNK_MODEL hero_fountain_base_model = {
#include "chaostghero/land/hero_fountain_base_model.inc"
};
NJS_CNK_OBJECT hero_fountain_base = {
#include "chaostghero/land/hero_fountain_base.inc"
};
static Sint16 hero_upper_pool_polygons[95] = {
#include "chaostghero/land/hero_upper_pool_polygons.inc"
};
static Sint32 hero_upper_pool_vertices[63] = {
#include "chaostghero/land/hero_upper_pool_vertices.inc"
};
static NJS_CNK_MODEL hero_upper_pool_model = {
#include "chaostghero/land/hero_upper_pool_model.inc"
};
NJS_CNK_OBJECT hero_upper_pool = {
#include "chaostghero/land/hero_upper_pool.inc"
};
static Sint16 hero_lower_pool_polygons[135] = {
#include "chaostghero/land/hero_lower_pool_polygons.inc"
};
static Sint32 hero_lower_pool_vertices[91] = {
#include "chaostghero/land/hero_lower_pool_vertices.inc"
};
static NJS_CNK_MODEL hero_lower_pool_model = {
#include "chaostghero/land/hero_lower_pool_model.inc"
};
NJS_CNK_OBJECT hero_lower_pool = {
#include "chaostghero/land/hero_lower_pool.inc"
};
static Sint16 draw_6_polygons[545] = {
#include "chaostghero/land/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[483] = {
#include "chaostghero/land/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "chaostghero/land/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "chaostghero/land/draw_6.inc"
};
static Sint16 draw_5_polygons[275] = {
#include "chaostghero/land/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[187] = {
#include "chaostghero/land/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "chaostghero/land/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "chaostghero/land/draw_5.inc"
};
static Sint16 draw_4_polygons[25] = {
#include "chaostghero/land/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[19] = {
#include "chaostghero/land/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "chaostghero/land/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "chaostghero/land/draw_4.inc"
};
static Sint16 draw_3_polygons[91] = {
#include "chaostghero/land/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[99] = {
#include "chaostghero/land/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "chaostghero/land/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "chaostghero/land/draw_3.inc"
};
static Sint16 draw_2_polygons[25] = {
#include "chaostghero/land/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[19] = {
#include "chaostghero/land/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "chaostghero/land/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "chaostghero/land/draw_2.inc"
};
static Sint16 hero_fountain_jet_a_polygons[63] = {
#include "chaostghero/land/hero_fountain_jet_a_polygons.inc"
};
static Sint32 hero_fountain_jet_a_vertices[59] = {
#include "chaostghero/land/hero_fountain_jet_a_vertices.inc"
};
static NJS_CNK_MODEL hero_fountain_jet_a_model = {
#include "chaostghero/land/hero_fountain_jet_a_model.inc"
};
NJS_CNK_OBJECT hero_fountain_jet_a = {
#include "chaostghero/land/hero_fountain_jet_a.inc"
};
static Sint16 hero_fountain_jet_b_polygons[63] = {
#include "chaostghero/land/hero_fountain_jet_b_polygons.inc"
};
static Sint32 hero_fountain_jet_b_vertices[59] = {
#include "chaostghero/land/hero_fountain_jet_b_vertices.inc"
};
static NJS_CNK_MODEL hero_fountain_jet_b_model = {
#include "chaostghero/land/hero_fountain_jet_b_model.inc"
};
NJS_CNK_OBJECT hero_fountain_jet_b = {
#include "chaostghero/land/hero_fountain_jet_b.inc"
};
static Sint16 hero_fountain_jet_c_polygons[63] = {
#include "chaostghero/land/hero_fountain_jet_c_polygons.inc"
};
static Sint32 hero_fountain_jet_c_vertices[59] = {
#include "chaostghero/land/hero_fountain_jet_c_vertices.inc"
};
static NJS_CNK_MODEL hero_fountain_jet_c_model = {
#include "chaostghero/land/hero_fountain_jet_c_model.inc"
};
NJS_CNK_OBJECT hero_fountain_jet_c = {
#include "chaostghero/land/hero_fountain_jet_c.inc"
};
static Sint16 hero_fountain_jet_d_polygons[63] = {
#include "chaostghero/land/hero_fountain_jet_d_polygons.inc"
};
static Sint32 hero_fountain_jet_d_vertices[59] = {
#include "chaostghero/land/hero_fountain_jet_d_vertices.inc"
};
static NJS_CNK_MODEL hero_fountain_jet_d_model = {
#include "chaostghero/land/hero_fountain_jet_d_model.inc"
};
NJS_CNK_OBJECT hero_fountain_jet_d = {
#include "chaostghero/land/hero_fountain_jet_d.inc"
};
static Sint16 draw_1_polygons[1333] = {
#include "chaostghero/land/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[1355] = {
#include "chaostghero/land/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "chaostghero/land/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "chaostghero/land/draw_1.inc"
};
static Sint16 draw_0_polygons[253] = {
#include "chaostghero/land/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[283] = {
#include "chaostghero/land/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "chaostghero/land/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "chaostghero/land/draw_0.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "chaostghero/land/collision_28_materials.inc"
};
static Sint16 collision_28_faces[31] = {
#include "chaostghero/land/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "chaostghero/land/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[15] = {
#include "chaostghero/land/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "chaostghero/land/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "chaostghero/land/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "chaostghero/land/collision_29_materials.inc"
};
static Sint16 collision_29_faces[46] = {
#include "chaostghero/land/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "chaostghero/land/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[21] = {
#include "chaostghero/land/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "chaostghero/land/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "chaostghero/land/collision_29.inc"
};
static struct land_entry entries[63] = {
#include "chaostghero/land/entries.inc"
};
struct land_table hero_land = {
#include "chaostghero/land/hero_land.inc"
};
