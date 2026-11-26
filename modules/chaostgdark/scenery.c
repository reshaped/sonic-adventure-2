/* CHAOSTGDARK.PRS's texture lists, chunk models, Basic models, land and
 * texture names, from the disc. its C lies between garden_prop_editor.c and
 * objects.c, and its D lies between prop_models.c and objects.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgdark/scenery.h"
#include "collision/world.h"

#include "chaostgdark/scenery/text.inc"

static NJS_TEXNAME textures[27] = {
#include "chaostgdark/scenery/textures.inc"
};
NJS_TEXLIST dark_land_texlist = {
#include "chaostgdark/scenery/dark_land_texlist.inc"
};
static Sint16 draw_35_polygons[181] = {
#include "chaostgdark/scenery/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[119] = {
#include "chaostgdark/scenery/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "chaostgdark/scenery/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "chaostgdark/scenery/draw_35.inc"
};
static Sint16 draw_34_polygons[25] = {
#include "chaostgdark/scenery/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[19] = {
#include "chaostgdark/scenery/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "chaostgdark/scenery/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "chaostgdark/scenery/draw_34.inc"
};
static Sint16 draw_33_polygons[25] = {
#include "chaostgdark/scenery/draw_33_polygons.inc"
};
static Sint32 draw_33_vertices[19] = {
#include "chaostgdark/scenery/draw_33_vertices.inc"
};
static NJS_CNK_MODEL draw_33_model = {
#include "chaostgdark/scenery/draw_33_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "chaostgdark/scenery/draw_33.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "chaostgdark/scenery/collision_0_materials.inc"
};
static Sint16 collision_0_faces[21] = {
#include "chaostgdark/scenery/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "chaostgdark/scenery/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[12] = {
#include "chaostgdark/scenery/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "chaostgdark/scenery/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "chaostgdark/scenery/collision_0.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "chaostgdark/scenery/collision_1_materials.inc"
};
static Sint16 collision_1_faces[63] = {
#include "chaostgdark/scenery/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "chaostgdark/scenery/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[27] = {
#include "chaostgdark/scenery/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "chaostgdark/scenery/collision_1_model.inc"
};
static NJS_OBJECT collision_1 = {
#include "chaostgdark/scenery/collision_1.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "chaostgdark/scenery/collision_2_materials.inc"
};
static Sint16 collision_2_faces[63] = {
#include "chaostgdark/scenery/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "chaostgdark/scenery/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[29] = {
#include "chaostgdark/scenery/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "chaostgdark/scenery/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "chaostgdark/scenery/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "chaostgdark/scenery/collision_3_materials.inc"
};
static Sint16 collision_3_faces[49] = {
#include "chaostgdark/scenery/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "chaostgdark/scenery/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[34] = {
#include "chaostgdark/scenery/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "chaostgdark/scenery/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "chaostgdark/scenery/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "chaostgdark/scenery/collision_4_materials.inc"
};
static Sint16 collision_4_faces[52] = {
#include "chaostgdark/scenery/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "chaostgdark/scenery/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[28] = {
#include "chaostgdark/scenery/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "chaostgdark/scenery/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "chaostgdark/scenery/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "chaostgdark/scenery/collision_5_materials.inc"
};
static Sint16 collision_5_faces[13] = {
#include "chaostgdark/scenery/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "chaostgdark/scenery/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[12] = {
#include "chaostgdark/scenery/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "chaostgdark/scenery/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "chaostgdark/scenery/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "chaostgdark/scenery/collision_6_materials.inc"
};
static Sint16 collision_6_faces[5] = {
#include "chaostgdark/scenery/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "chaostgdark/scenery/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[4] = {
#include "chaostgdark/scenery/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "chaostgdark/scenery/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "chaostgdark/scenery/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "chaostgdark/scenery/collision_7_materials.inc"
};
static Sint16 collision_7_faces[40] = {
#include "chaostgdark/scenery/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "chaostgdark/scenery/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[25] = {
#include "chaostgdark/scenery/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "chaostgdark/scenery/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "chaostgdark/scenery/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "chaostgdark/scenery/collision_8_materials.inc"
};
static Sint16 collision_8_faces[45] = {
#include "chaostgdark/scenery/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "chaostgdark/scenery/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[22] = {
#include "chaostgdark/scenery/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "chaostgdark/scenery/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "chaostgdark/scenery/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "chaostgdark/scenery/collision_9_materials.inc"
};
static Sint16 collision_9_faces[52] = {
#include "chaostgdark/scenery/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "chaostgdark/scenery/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[39] = {
#include "chaostgdark/scenery/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "chaostgdark/scenery/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "chaostgdark/scenery/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "chaostgdark/scenery/collision_10_materials.inc"
};
static Sint16 collision_10_faces[64] = {
#include "chaostgdark/scenery/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "chaostgdark/scenery/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[38] = {
#include "chaostgdark/scenery/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "chaostgdark/scenery/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "chaostgdark/scenery/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "chaostgdark/scenery/collision_11_materials.inc"
};
static Sint16 collision_11_faces[59] = {
#include "chaostgdark/scenery/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "chaostgdark/scenery/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[22] = {
#include "chaostgdark/scenery/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "chaostgdark/scenery/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "chaostgdark/scenery/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "chaostgdark/scenery/collision_12_materials.inc"
};
static Sint16 collision_12_faces[48] = {
#include "chaostgdark/scenery/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "chaostgdark/scenery/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[23] = {
#include "chaostgdark/scenery/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "chaostgdark/scenery/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "chaostgdark/scenery/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "chaostgdark/scenery/collision_13_materials.inc"
};
static Sint16 collision_13_faces[23] = {
#include "chaostgdark/scenery/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "chaostgdark/scenery/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[16] = {
#include "chaostgdark/scenery/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "chaostgdark/scenery/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "chaostgdark/scenery/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "chaostgdark/scenery/collision_14_materials.inc"
};
static Sint16 collision_14_faces[31] = {
#include "chaostgdark/scenery/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "chaostgdark/scenery/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[18] = {
#include "chaostgdark/scenery/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "chaostgdark/scenery/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "chaostgdark/scenery/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "chaostgdark/scenery/collision_15_materials.inc"
};
static Sint16 collision_15_faces[48] = {
#include "chaostgdark/scenery/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "chaostgdark/scenery/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[24] = {
#include "chaostgdark/scenery/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "chaostgdark/scenery/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "chaostgdark/scenery/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "chaostgdark/scenery/collision_16_materials.inc"
};
static Sint16 collision_16_faces[31] = {
#include "chaostgdark/scenery/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "chaostgdark/scenery/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[15] = {
#include "chaostgdark/scenery/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "chaostgdark/scenery/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "chaostgdark/scenery/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "chaostgdark/scenery/collision_17_materials.inc"
};
static Sint16 collision_17_faces[25] = {
#include "chaostgdark/scenery/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "chaostgdark/scenery/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[15] = {
#include "chaostgdark/scenery/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "chaostgdark/scenery/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "chaostgdark/scenery/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "chaostgdark/scenery/collision_18_materials.inc"
};
static Sint16 collision_18_faces[30] = {
#include "chaostgdark/scenery/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "chaostgdark/scenery/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[21] = {
#include "chaostgdark/scenery/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "chaostgdark/scenery/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "chaostgdark/scenery/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "chaostgdark/scenery/collision_19_materials.inc"
};
static Sint16 collision_19_faces[26] = {
#include "chaostgdark/scenery/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "chaostgdark/scenery/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[18] = {
#include "chaostgdark/scenery/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "chaostgdark/scenery/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "chaostgdark/scenery/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "chaostgdark/scenery/collision_20_materials.inc"
};
static Sint16 collision_20_faces[26] = {
#include "chaostgdark/scenery/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "chaostgdark/scenery/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[18] = {
#include "chaostgdark/scenery/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "chaostgdark/scenery/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "chaostgdark/scenery/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "chaostgdark/scenery/collision_21_materials.inc"
};
static Sint16 collision_21_faces[26] = {
#include "chaostgdark/scenery/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "chaostgdark/scenery/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[18] = {
#include "chaostgdark/scenery/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "chaostgdark/scenery/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "chaostgdark/scenery/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "chaostgdark/scenery/collision_22_materials.inc"
};
static Sint16 collision_22_faces[110] = {
#include "chaostgdark/scenery/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "chaostgdark/scenery/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[68] = {
#include "chaostgdark/scenery/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "chaostgdark/scenery/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "chaostgdark/scenery/collision_22.inc"
};
static Sint16 draw_32_polygons[359] = {
#include "chaostgdark/scenery/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[263] = {
#include "chaostgdark/scenery/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "chaostgdark/scenery/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "chaostgdark/scenery/draw_32.inc"
};
static Sint16 draw_31_polygons[493] = {
#include "chaostgdark/scenery/draw_31_polygons.inc"
};
static Sint32 draw_31_vertices[323] = {
#include "chaostgdark/scenery/draw_31_vertices.inc"
};
static NJS_CNK_MODEL draw_31_model = {
#include "chaostgdark/scenery/draw_31_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "chaostgdark/scenery/draw_31.inc"
};
static Sint16 draw_30_polygons[237] = {
#include "chaostgdark/scenery/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[183] = {
#include "chaostgdark/scenery/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "chaostgdark/scenery/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "chaostgdark/scenery/draw_30.inc"
};
static Sint16 draw_29_polygons[85] = {
#include "chaostgdark/scenery/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[63] = {
#include "chaostgdark/scenery/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "chaostgdark/scenery/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "chaostgdark/scenery/draw_29.inc"
};
static Sint16 draw_28_polygons[141] = {
#include "chaostgdark/scenery/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[99] = {
#include "chaostgdark/scenery/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "chaostgdark/scenery/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "chaostgdark/scenery/draw_28.inc"
};
static Sint16 draw_27_polygons[173] = {
#include "chaostgdark/scenery/draw_27_polygons.inc"
};
static Sint32 draw_27_vertices[115] = {
#include "chaostgdark/scenery/draw_27_vertices.inc"
};
static NJS_CNK_MODEL draw_27_model = {
#include "chaostgdark/scenery/draw_27_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "chaostgdark/scenery/draw_27.inc"
};
static Sint16 draw_26_polygons[227] = {
#include "chaostgdark/scenery/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[195] = {
#include "chaostgdark/scenery/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "chaostgdark/scenery/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "chaostgdark/scenery/draw_26.inc"
};
static Sint16 draw_25_polygons[211] = {
#include "chaostgdark/scenery/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[175] = {
#include "chaostgdark/scenery/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "chaostgdark/scenery/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "chaostgdark/scenery/draw_25.inc"
};
static Sint16 draw_24_polygons[317] = {
#include "chaostgdark/scenery/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[259] = {
#include "chaostgdark/scenery/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "chaostgdark/scenery/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "chaostgdark/scenery/draw_24.inc"
};
static Sint16 draw_23_polygons[187] = {
#include "chaostgdark/scenery/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[143] = {
#include "chaostgdark/scenery/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "chaostgdark/scenery/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "chaostgdark/scenery/draw_23.inc"
};
static Sint16 draw_22_polygons[77] = {
#include "chaostgdark/scenery/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[51] = {
#include "chaostgdark/scenery/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "chaostgdark/scenery/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "chaostgdark/scenery/draw_22.inc"
};
static Sint16 draw_21_polygons[261] = {
#include "chaostgdark/scenery/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[179] = {
#include "chaostgdark/scenery/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "chaostgdark/scenery/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "chaostgdark/scenery/draw_21.inc"
};
static Sint16 draw_20_polygons[77] = {
#include "chaostgdark/scenery/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[47] = {
#include "chaostgdark/scenery/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "chaostgdark/scenery/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "chaostgdark/scenery/draw_20.inc"
};
static Sint16 dark_water_a_polygons[87] = {
#include "chaostgdark/scenery/dark_water_a_polygons.inc"
};
static Sint32 dark_water_a_vertices[67] = {
#include "chaostgdark/scenery/dark_water_a_vertices.inc"
};
static NJS_CNK_MODEL dark_water_a_model = {
#include "chaostgdark/scenery/dark_water_a_model.inc"
};
NJS_CNK_OBJECT dark_water_a = {
#include "chaostgdark/scenery/dark_water_a.inc"
};
static Sint16 dark_water_b_polygons[177] = {
#include "chaostgdark/scenery/dark_water_b_polygons.inc"
};
static Sint32 dark_water_b_vertices[127] = {
#include "chaostgdark/scenery/dark_water_b_vertices.inc"
};
static NJS_CNK_MODEL dark_water_b_model = {
#include "chaostgdark/scenery/dark_water_b_model.inc"
};
NJS_CNK_OBJECT dark_water_b = {
#include "chaostgdark/scenery/dark_water_b.inc"
};
static Sint16 draw_19_polygons[317] = {
#include "chaostgdark/scenery/draw_19_polygons.inc"
};
static Sint32 draw_19_vertices[219] = {
#include "chaostgdark/scenery/draw_19_vertices.inc"
};
static NJS_CNK_MODEL draw_19_model = {
#include "chaostgdark/scenery/draw_19_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "chaostgdark/scenery/draw_19.inc"
};
static Sint16 draw_18_polygons[297] = {
#include "chaostgdark/scenery/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[299] = {
#include "chaostgdark/scenery/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "chaostgdark/scenery/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "chaostgdark/scenery/draw_18.inc"
};
static Sint16 draw_17_polygons[255] = {
#include "chaostgdark/scenery/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[143] = {
#include "chaostgdark/scenery/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "chaostgdark/scenery/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "chaostgdark/scenery/draw_17.inc"
};
static Sint16 draw_16_polygons[337] = {
#include "chaostgdark/scenery/draw_16_polygons.inc"
};
static Sint32 draw_16_vertices[203] = {
#include "chaostgdark/scenery/draw_16_vertices.inc"
};
static NJS_CNK_MODEL draw_16_model = {
#include "chaostgdark/scenery/draw_16_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "chaostgdark/scenery/draw_16.inc"
};
static Sint16 draw_15_polygons[425] = {
#include "chaostgdark/scenery/draw_15_polygons.inc"
};
static Sint32 draw_15_vertices[239] = {
#include "chaostgdark/scenery/draw_15_vertices.inc"
};
static NJS_CNK_MODEL draw_15_model = {
#include "chaostgdark/scenery/draw_15_model.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "chaostgdark/scenery/draw_15.inc"
};
static Sint16 draw_14_polygons[145] = {
#include "chaostgdark/scenery/draw_14_polygons.inc"
};
static Sint32 draw_14_vertices[91] = {
#include "chaostgdark/scenery/draw_14_vertices.inc"
};
static NJS_CNK_MODEL draw_14_model = {
#include "chaostgdark/scenery/draw_14_model.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "chaostgdark/scenery/draw_14.inc"
};
static Sint16 draw_13_polygons[91] = {
#include "chaostgdark/scenery/draw_13_polygons.inc"
};
static Sint32 draw_13_vertices[99] = {
#include "chaostgdark/scenery/draw_13_vertices.inc"
};
static NJS_CNK_MODEL draw_13_model = {
#include "chaostgdark/scenery/draw_13_model.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "chaostgdark/scenery/draw_13.inc"
};
static Sint16 draw_12_polygons[211] = {
#include "chaostgdark/scenery/draw_12_polygons.inc"
};
static Sint32 draw_12_vertices[139] = {
#include "chaostgdark/scenery/draw_12_vertices.inc"
};
static NJS_CNK_MODEL draw_12_model = {
#include "chaostgdark/scenery/draw_12_model.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "chaostgdark/scenery/draw_12.inc"
};
static Sint16 draw_11_polygons[127] = {
#include "chaostgdark/scenery/draw_11_polygons.inc"
};
static Sint32 draw_11_vertices[83] = {
#include "chaostgdark/scenery/draw_11_vertices.inc"
};
static NJS_CNK_MODEL draw_11_model = {
#include "chaostgdark/scenery/draw_11_model.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "chaostgdark/scenery/draw_11.inc"
};
static Sint16 draw_10_polygons[189] = {
#include "chaostgdark/scenery/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[123] = {
#include "chaostgdark/scenery/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "chaostgdark/scenery/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "chaostgdark/scenery/draw_10.inc"
};
static Sint16 draw_9_polygons[385] = {
#include "chaostgdark/scenery/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[275] = {
#include "chaostgdark/scenery/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "chaostgdark/scenery/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "chaostgdark/scenery/draw_9.inc"
};
static Sint16 draw_8_polygons[181] = {
#include "chaostgdark/scenery/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[91] = {
#include "chaostgdark/scenery/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "chaostgdark/scenery/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "chaostgdark/scenery/draw_8.inc"
};
static Sint16 draw_7_polygons[1257] = {
#include "chaostgdark/scenery/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[915] = {
#include "chaostgdark/scenery/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "chaostgdark/scenery/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "chaostgdark/scenery/draw_7.inc"
};
static Sint16 draw_6_polygons[43] = {
#include "chaostgdark/scenery/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[35] = {
#include "chaostgdark/scenery/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "chaostgdark/scenery/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "chaostgdark/scenery/draw_6.inc"
};
static Sint16 draw_5_polygons[51] = {
#include "chaostgdark/scenery/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[51] = {
#include "chaostgdark/scenery/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "chaostgdark/scenery/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "chaostgdark/scenery/draw_5.inc"
};
static Sint16 draw_4_polygons[51] = {
#include "chaostgdark/scenery/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[51] = {
#include "chaostgdark/scenery/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "chaostgdark/scenery/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "chaostgdark/scenery/draw_4.inc"
};
static Sint16 draw_3_polygons[43] = {
#include "chaostgdark/scenery/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[35] = {
#include "chaostgdark/scenery/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "chaostgdark/scenery/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "chaostgdark/scenery/draw_3.inc"
};
static Sint16 draw_2_polygons[253] = {
#include "chaostgdark/scenery/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[123] = {
#include "chaostgdark/scenery/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "chaostgdark/scenery/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "chaostgdark/scenery/draw_2.inc"
};
static Sint16 draw_1_polygons[45] = {
#include "chaostgdark/scenery/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[31] = {
#include "chaostgdark/scenery/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "chaostgdark/scenery/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "chaostgdark/scenery/draw_1.inc"
};
static Sint16 draw_0_polygons[415] = {
#include "chaostgdark/scenery/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[223] = {
#include "chaostgdark/scenery/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "chaostgdark/scenery/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "chaostgdark/scenery/draw_0.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "chaostgdark/scenery/collision_23_materials.inc"
};
static Sint16 collision_23_faces[27] = {
#include "chaostgdark/scenery/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "chaostgdark/scenery/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[16] = {
#include "chaostgdark/scenery/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "chaostgdark/scenery/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "chaostgdark/scenery/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "chaostgdark/scenery/collision_24_materials.inc"
};
static Sint16 collision_24_faces[59] = {
#include "chaostgdark/scenery/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "chaostgdark/scenery/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[31] = {
#include "chaostgdark/scenery/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "chaostgdark/scenery/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "chaostgdark/scenery/collision_24.inc"
};
static struct land_entry entries[61] = {
#include "chaostgdark/scenery/entries.inc"
};
struct land_table dark_land = {
#include "chaostgdark/scenery/dark_land.inc"
};
