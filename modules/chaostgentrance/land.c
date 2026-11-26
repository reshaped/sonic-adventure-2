#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgentrance/land.h"
#include "collision/world.h"

/* the land's file names, which its land table and texture list point at:
 * each texture's, then their archive's */
const char entrance_land_filenames[624] = {
#include "chaostgentrance/land/entrance_land_filenames.inc"
};

/* CHAOSTGENTRANCE.PRS's texture lists, Basic models, chunk models and land,
 * from the disc. its D lies between camera.c and object_light.c */
static NJS_TEXNAME entrance_land_textures[46] = {
#include "chaostgentrance/land/entrance_land_textures.inc"
};
NJS_TEXLIST entrance_land_texlist = {
#include "chaostgentrance/land/entrance_land_texlist.inc"
};
static NJS_MATERIAL entrance_land_collision_1_materials[1] = {
#include "chaostgentrance/land/entrance_land_collision_1_materials.inc"
};
static Sint16 entrance_land_collision_1_faces[5] = {
#include "chaostgentrance/land/entrance_land_collision_1_faces.inc"
};
static NJS_MESHSET entrance_land_collision_1_meshsets[1] = {
#include "chaostgentrance/land/entrance_land_collision_1_meshsets.inc"
};
static NJS_POINT3 entrance_land_collision_1_points[4] = {
#include "chaostgentrance/land/entrance_land_collision_1_points.inc"
};
static NJS_MODEL entrance_land_collision_1_model = {
#include "chaostgentrance/land/entrance_land_collision_1_model.inc"
};
static NJS_OBJECT entrance_land_collision_1 = {
#include "chaostgentrance/land/entrance_land_collision_1.inc"
};
static NJS_MATERIAL entrance_land_collision_2_materials[1] = {
#include "chaostgentrance/land/entrance_land_collision_2_materials.inc"
};
static Sint16 entrance_land_collision_2_faces[8] = {
#include "chaostgentrance/land/entrance_land_collision_2_faces.inc"
};
static NJS_MESHSET entrance_land_collision_2_meshsets[1] = {
#include "chaostgentrance/land/entrance_land_collision_2_meshsets.inc"
};
static NJS_POINT3 entrance_land_collision_2_points[7] = {
#include "chaostgentrance/land/entrance_land_collision_2_points.inc"
};
static NJS_MODEL entrance_land_collision_2_model = {
#include "chaostgentrance/land/entrance_land_collision_2_model.inc"
};
static NJS_OBJECT entrance_land_collision_2 = {
#include "chaostgentrance/land/entrance_land_collision_2.inc"
};
static Sint16 entrance_land_draw_3_polygons[459] = {
#include "chaostgentrance/land/entrance_land_draw_3_polygons.inc"
};
static Sint32 entrance_land_draw_3_vertices[479] = {
#include "chaostgentrance/land/entrance_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_3_model = {
#include "chaostgentrance/land/entrance_land_draw_3_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_3 = {
#include "chaostgentrance/land/entrance_land_draw_3.inc"
};
static Sint16 entrance_land_draw_4_polygons[443] = {
#include "chaostgentrance/land/entrance_land_draw_4_polygons.inc"
};
static Sint32 entrance_land_draw_4_vertices[451] = {
#include "chaostgentrance/land/entrance_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_4_model = {
#include "chaostgentrance/land/entrance_land_draw_4_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_4 = {
#include "chaostgentrance/land/entrance_land_draw_4.inc"
};
static Sint16 entrance_land_draw_5_polygons[555] = {
#include "chaostgentrance/land/entrance_land_draw_5_polygons.inc"
};
static Sint32 entrance_land_draw_5_vertices[507] = {
#include "chaostgentrance/land/entrance_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_5_model = {
#include "chaostgentrance/land/entrance_land_draw_5_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_5 = {
#include "chaostgentrance/land/entrance_land_draw_5.inc"
};
static Sint16 entrance_land_draw_6_polygons[2045] = {
#include "chaostgentrance/land/entrance_land_draw_6_polygons.inc"
};
static Sint32 entrance_land_draw_6_vertices[1915] = {
#include "chaostgentrance/land/entrance_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_6_model = {
#include "chaostgentrance/land/entrance_land_draw_6_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_6 = {
#include "chaostgentrance/land/entrance_land_draw_6.inc"
};
static Sint16 entrance_land_draw_7_polygons[247] = {
#include "chaostgentrance/land/entrance_land_draw_7_polygons.inc"
};
static Sint32 entrance_land_draw_7_vertices[271] = {
#include "chaostgentrance/land/entrance_land_draw_7_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_7_model = {
#include "chaostgentrance/land/entrance_land_draw_7_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_7 = {
#include "chaostgentrance/land/entrance_land_draw_7.inc"
};
static Sint16 entrance_land_draw_8_polygons[383] = {
#include "chaostgentrance/land/entrance_land_draw_8_polygons.inc"
};
static Sint32 entrance_land_draw_8_vertices[371] = {
#include "chaostgentrance/land/entrance_land_draw_8_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_8_model = {
#include "chaostgentrance/land/entrance_land_draw_8_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_8 = {
#include "chaostgentrance/land/entrance_land_draw_8.inc"
};
static Sint16 entrance_land_draw_9_polygons[383] = {
#include "chaostgentrance/land/entrance_land_draw_9_polygons.inc"
};
static Sint32 entrance_land_draw_9_vertices[359] = {
#include "chaostgentrance/land/entrance_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_9_model = {
#include "chaostgentrance/land/entrance_land_draw_9_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_9 = {
#include "chaostgentrance/land/entrance_land_draw_9.inc"
};
static Sint16 entrance_land_draw_10_polygons[215] = {
#include "chaostgentrance/land/entrance_land_draw_10_polygons.inc"
};
static Sint32 entrance_land_draw_10_vertices[219] = {
#include "chaostgentrance/land/entrance_land_draw_10_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_10_model = {
#include "chaostgentrance/land/entrance_land_draw_10_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_10 = {
#include "chaostgentrance/land/entrance_land_draw_10.inc"
};
static Sint16 entrance_land_draw_11_polygons[493] = {
#include "chaostgentrance/land/entrance_land_draw_11_polygons.inc"
};
static Sint32 entrance_land_draw_11_vertices[527] = {
#include "chaostgentrance/land/entrance_land_draw_11_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_11_model = {
#include "chaostgentrance/land/entrance_land_draw_11_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_11 = {
#include "chaostgentrance/land/entrance_land_draw_11.inc"
};
static Sint16 entrance_land_draw_12_polygons[1085] = {
#include "chaostgentrance/land/entrance_land_draw_12_polygons.inc"
};
static Sint32 entrance_land_draw_12_vertices[1131] = {
#include "chaostgentrance/land/entrance_land_draw_12_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_12_model = {
#include "chaostgentrance/land/entrance_land_draw_12_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_12 = {
#include "chaostgentrance/land/entrance_land_draw_12.inc"
};
static Sint16 entrance_land_draw_13_polygons[833] = {
#include "chaostgentrance/land/entrance_land_draw_13_polygons.inc"
};
static Sint32 entrance_land_draw_13_vertices[903] = {
#include "chaostgentrance/land/entrance_land_draw_13_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_13_model = {
#include "chaostgentrance/land/entrance_land_draw_13_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_13 = {
#include "chaostgentrance/land/entrance_land_draw_13.inc"
};
static Sint16 entrance_land_draw_14_polygons[449] = {
#include "chaostgentrance/land/entrance_land_draw_14_polygons.inc"
};
static Sint32 entrance_land_draw_14_vertices[451] = {
#include "chaostgentrance/land/entrance_land_draw_14_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_14_model = {
#include "chaostgentrance/land/entrance_land_draw_14_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_14 = {
#include "chaostgentrance/land/entrance_land_draw_14.inc"
};
static Sint16 entrance_land_draw_0_polygons[449] = {
#include "chaostgentrance/land/entrance_land_draw_0_polygons.inc"
};
static Sint32 entrance_land_draw_0_vertices[451] = {
#include "chaostgentrance/land/entrance_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_0_model = {
#include "chaostgentrance/land/entrance_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_17 = {
#include "chaostgentrance/land/entrance_land_draw_17.inc"
};
static NJS_MATERIAL entrance_land_collision_19_materials[1] = {
#include "chaostgentrance/land/entrance_land_collision_19_materials.inc"
};
static Sint16 entrance_land_collision_19_faces[42] = {
#include "chaostgentrance/land/entrance_land_collision_19_faces.inc"
};
static NJS_MESHSET entrance_land_collision_19_meshsets[1] = {
#include "chaostgentrance/land/entrance_land_collision_19_meshsets.inc"
};
static NJS_POINT3 entrance_land_collision_19_points[32] = {
#include "chaostgentrance/land/entrance_land_collision_19_points.inc"
};
static NJS_MODEL entrance_land_collision_19_model = {
#include "chaostgentrance/land/entrance_land_collision_19_model.inc"
};
static NJS_OBJECT entrance_land_collision_19 = {
#include "chaostgentrance/land/entrance_land_collision_19.inc"
};
static Sint16 entrance_land_draw_20_polygons[87] = {
#include "chaostgentrance/land/entrance_land_draw_20_polygons.inc"
};
static Sint32 entrance_land_draw_20_vertices[75] = {
#include "chaostgentrance/land/entrance_land_draw_20_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_20_model = {
#include "chaostgentrance/land/entrance_land_draw_20_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_20 = {
#include "chaostgentrance/land/entrance_land_draw_20.inc"
};
static Sint16 entrance_land_draw_21_polygons[583] = {
#include "chaostgentrance/land/entrance_land_draw_21_polygons.inc"
};
static Sint32 entrance_land_draw_21_vertices[491] = {
#include "chaostgentrance/land/entrance_land_draw_21_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_21_model = {
#include "chaostgentrance/land/entrance_land_draw_21_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_21 = {
#include "chaostgentrance/land/entrance_land_draw_21.inc"
};
static Sint16 entrance_land_draw_22_polygons[469] = {
#include "chaostgentrance/land/entrance_land_draw_22_polygons.inc"
};
static Sint32 entrance_land_draw_22_vertices[375] = {
#include "chaostgentrance/land/entrance_land_draw_22_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_22_model = {
#include "chaostgentrance/land/entrance_land_draw_22_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_22 = {
#include "chaostgentrance/land/entrance_land_draw_22.inc"
};
static Sint16 entrance_land_draw_23_polygons[349] = {
#include "chaostgentrance/land/entrance_land_draw_23_polygons.inc"
};
static Sint32 entrance_land_draw_23_vertices[375] = {
#include "chaostgentrance/land/entrance_land_draw_23_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_23_model = {
#include "chaostgentrance/land/entrance_land_draw_23_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_23 = {
#include "chaostgentrance/land/entrance_land_draw_23.inc"
};
static Sint16 entrance_land_draw_24_polygons[361] = {
#include "chaostgentrance/land/entrance_land_draw_24_polygons.inc"
};
static Sint32 entrance_land_draw_24_vertices[391] = {
#include "chaostgentrance/land/entrance_land_draw_24_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_24_model = {
#include "chaostgentrance/land/entrance_land_draw_24_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_24 = {
#include "chaostgentrance/land/entrance_land_draw_24.inc"
};
static Sint16 entrance_land_draw_25_polygons[361] = {
#include "chaostgentrance/land/entrance_land_draw_25_polygons.inc"
};
static Sint32 entrance_land_draw_25_vertices[379] = {
#include "chaostgentrance/land/entrance_land_draw_25_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_25_model = {
#include "chaostgentrance/land/entrance_land_draw_25_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_25 = {
#include "chaostgentrance/land/entrance_land_draw_25.inc"
};
static Sint16 entrance_land_draw_26_polygons[147] = {
#include "chaostgentrance/land/entrance_land_draw_26_polygons.inc"
};
static Sint32 entrance_land_draw_26_vertices[151] = {
#include "chaostgentrance/land/entrance_land_draw_26_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_26_model = {
#include "chaostgentrance/land/entrance_land_draw_26_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_26 = {
#include "chaostgentrance/land/entrance_land_draw_26.inc"
};
static Sint16 entrance_land_draw_27_polygons[239] = {
#include "chaostgentrance/land/entrance_land_draw_27_polygons.inc"
};
static Sint32 entrance_land_draw_27_vertices[267] = {
#include "chaostgentrance/land/entrance_land_draw_27_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_27_model = {
#include "chaostgentrance/land/entrance_land_draw_27_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_27 = {
#include "chaostgentrance/land/entrance_land_draw_27.inc"
};
static Sint16 entrance_land_draw_28_polygons[309] = {
#include "chaostgentrance/land/entrance_land_draw_28_polygons.inc"
};
static Sint32 entrance_land_draw_28_vertices[339] = {
#include "chaostgentrance/land/entrance_land_draw_28_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_28_model = {
#include "chaostgentrance/land/entrance_land_draw_28_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_28 = {
#include "chaostgentrance/land/entrance_land_draw_28.inc"
};
static Sint16 entrance_land_draw_29_polygons[1475] = {
#include "chaostgentrance/land/entrance_land_draw_29_polygons.inc"
};
static Sint32 entrance_land_draw_29_vertices[1411] = {
#include "chaostgentrance/land/entrance_land_draw_29_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_29_model = {
#include "chaostgentrance/land/entrance_land_draw_29_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_29 = {
#include "chaostgentrance/land/entrance_land_draw_29.inc"
};
static Sint16 entrance_land_draw_30_polygons[329] = {
#include "chaostgentrance/land/entrance_land_draw_30_polygons.inc"
};
static Sint32 entrance_land_draw_30_vertices[343] = {
#include "chaostgentrance/land/entrance_land_draw_30_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_30_model = {
#include "chaostgentrance/land/entrance_land_draw_30_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_30 = {
#include "chaostgentrance/land/entrance_land_draw_30.inc"
};
static Sint16 entrance_land_draw_31_polygons[321] = {
#include "chaostgentrance/land/entrance_land_draw_31_polygons.inc"
};
static Sint32 entrance_land_draw_31_vertices[335] = {
#include "chaostgentrance/land/entrance_land_draw_31_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_31_model = {
#include "chaostgentrance/land/entrance_land_draw_31_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_31 = {
#include "chaostgentrance/land/entrance_land_draw_31.inc"
};
static Sint16 entrance_land_draw_32_polygons[253] = {
#include "chaostgentrance/land/entrance_land_draw_32_polygons.inc"
};
static Sint32 entrance_land_draw_32_vertices[275] = {
#include "chaostgentrance/land/entrance_land_draw_32_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_32_model = {
#include "chaostgentrance/land/entrance_land_draw_32_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_32 = {
#include "chaostgentrance/land/entrance_land_draw_32.inc"
};
static Sint16 entrance_land_draw_33_polygons[443] = {
#include "chaostgentrance/land/entrance_land_draw_33_polygons.inc"
};
static Sint32 entrance_land_draw_33_vertices[423] = {
#include "chaostgentrance/land/entrance_land_draw_33_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_33_model = {
#include "chaostgentrance/land/entrance_land_draw_33_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_33 = {
#include "chaostgentrance/land/entrance_land_draw_33.inc"
};
static Sint16 entrance_land_draw_34_polygons[445] = {
#include "chaostgentrance/land/entrance_land_draw_34_polygons.inc"
};
static Sint32 entrance_land_draw_34_vertices[431] = {
#include "chaostgentrance/land/entrance_land_draw_34_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_34_model = {
#include "chaostgentrance/land/entrance_land_draw_34_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_34 = {
#include "chaostgentrance/land/entrance_land_draw_34.inc"
};
static Sint16 entrance_land_draw_35_polygons[697] = {
#include "chaostgentrance/land/entrance_land_draw_35_polygons.inc"
};
static Sint32 entrance_land_draw_35_vertices[807] = {
#include "chaostgentrance/land/entrance_land_draw_35_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_35_model = {
#include "chaostgentrance/land/entrance_land_draw_35_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_35 = {
#include "chaostgentrance/land/entrance_land_draw_35.inc"
};
static Sint16 entrance_land_draw_36_polygons[549] = {
#include "chaostgentrance/land/entrance_land_draw_36_polygons.inc"
};
static Sint32 entrance_land_draw_36_vertices[499] = {
#include "chaostgentrance/land/entrance_land_draw_36_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_36_model = {
#include "chaostgentrance/land/entrance_land_draw_36_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_36 = {
#include "chaostgentrance/land/entrance_land_draw_36.inc"
};
static Sint16 entrance_land_draw_37_polygons[1947] = {
#include "chaostgentrance/land/entrance_land_draw_37_polygons.inc"
};
static Sint32 entrance_land_draw_37_vertices[1787] = {
#include "chaostgentrance/land/entrance_land_draw_37_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_37_model = {
#include "chaostgentrance/land/entrance_land_draw_37_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_37 = {
#include "chaostgentrance/land/entrance_land_draw_37.inc"
};
static Sint16 entrance_land_draw_38_polygons[383] = {
#include "chaostgentrance/land/entrance_land_draw_38_polygons.inc"
};
static Sint32 entrance_land_draw_38_vertices[379] = {
#include "chaostgentrance/land/entrance_land_draw_38_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_38_model = {
#include "chaostgentrance/land/entrance_land_draw_38_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_38 = {
#include "chaostgentrance/land/entrance_land_draw_38.inc"
};
static Sint16 entrance_land_draw_39_polygons[375] = {
#include "chaostgentrance/land/entrance_land_draw_39_polygons.inc"
};
static Sint32 entrance_land_draw_39_vertices[383] = {
#include "chaostgentrance/land/entrance_land_draw_39_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_39_model = {
#include "chaostgentrance/land/entrance_land_draw_39_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_39 = {
#include "chaostgentrance/land/entrance_land_draw_39.inc"
};
static Sint16 entrance_land_draw_40_polygons[485] = {
#include "chaostgentrance/land/entrance_land_draw_40_polygons.inc"
};
static Sint32 entrance_land_draw_40_vertices[515] = {
#include "chaostgentrance/land/entrance_land_draw_40_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_40_model = {
#include "chaostgentrance/land/entrance_land_draw_40_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_40 = {
#include "chaostgentrance/land/entrance_land_draw_40.inc"
};
static Sint16 entrance_land_draw_41_polygons[213] = {
#include "chaostgentrance/land/entrance_land_draw_41_polygons.inc"
};
static Sint32 entrance_land_draw_41_vertices[139] = {
#include "chaostgentrance/land/entrance_land_draw_41_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_41_model = {
#include "chaostgentrance/land/entrance_land_draw_41_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_41 = {
#include "chaostgentrance/land/entrance_land_draw_41.inc"
};
static Sint16 entrance_land_draw_42_polygons[385] = {
#include "chaostgentrance/land/entrance_land_draw_42_polygons.inc"
};
static Sint32 entrance_land_draw_42_vertices[399] = {
#include "chaostgentrance/land/entrance_land_draw_42_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_42_model = {
#include "chaostgentrance/land/entrance_land_draw_42_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_42 = {
#include "chaostgentrance/land/entrance_land_draw_42.inc"
};
static Sint16 entrance_land_draw_43_polygons[201] = {
#include "chaostgentrance/land/entrance_land_draw_43_polygons.inc"
};
static Sint32 entrance_land_draw_43_vertices[151] = {
#include "chaostgentrance/land/entrance_land_draw_43_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_43_model = {
#include "chaostgentrance/land/entrance_land_draw_43_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_43 = {
#include "chaostgentrance/land/entrance_land_draw_43.inc"
};
static Sint16 entrance_land_draw_44_polygons[209] = {
#include "chaostgentrance/land/entrance_land_draw_44_polygons.inc"
};
static Sint32 entrance_land_draw_44_vertices[187] = {
#include "chaostgentrance/land/entrance_land_draw_44_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_44_model = {
#include "chaostgentrance/land/entrance_land_draw_44_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_44 = {
#include "chaostgentrance/land/entrance_land_draw_44.inc"
};
static Sint16 entrance_land_draw_45_polygons[889] = {
#include "chaostgentrance/land/entrance_land_draw_45_polygons.inc"
};
static Sint32 entrance_land_draw_45_vertices[619] = {
#include "chaostgentrance/land/entrance_land_draw_45_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_45_model = {
#include "chaostgentrance/land/entrance_land_draw_45_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_45 = {
#include "chaostgentrance/land/entrance_land_draw_45.inc"
};
static Sint16 entrance_land_draw_46_polygons[57] = {
#include "chaostgentrance/land/entrance_land_draw_46_polygons.inc"
};
static Sint32 entrance_land_draw_46_vertices[39] = {
#include "chaostgentrance/land/entrance_land_draw_46_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_46_model = {
#include "chaostgentrance/land/entrance_land_draw_46_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_46 = {
#include "chaostgentrance/land/entrance_land_draw_46.inc"
};
static Sint16 entrance_land_backdrop_polygons[25] = {
#include "chaostgentrance/land/entrance_land_backdrop_polygons.inc"
};
static Sint32 entrance_land_backdrop_vertices[27] = {
#include "chaostgentrance/land/entrance_land_backdrop_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_backdrop_model = {
#include "chaostgentrance/land/entrance_land_backdrop_model.inc"
};
NJS_CNK_OBJECT entrance_backdrop = {
#include "chaostgentrance/land/entrance_backdrop.inc"
};
static Sint16 entrance_land_draw_56_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_56_polygons.inc"
};
static Sint32 entrance_land_draw_56_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_56_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_56_model = {
#include "chaostgentrance/land/entrance_land_draw_56_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_56 = {
#include "chaostgentrance/land/entrance_land_draw_56.inc"
};
static Sint16 entrance_land_draw_57_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_57_polygons.inc"
};
static Sint32 entrance_land_draw_57_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_57_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_57_model = {
#include "chaostgentrance/land/entrance_land_draw_57_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_57 = {
#include "chaostgentrance/land/entrance_land_draw_57.inc"
};
static Sint16 entrance_land_draw_58_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_58_polygons.inc"
};
static Sint32 entrance_land_draw_58_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_58_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_58_model = {
#include "chaostgentrance/land/entrance_land_draw_58_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_58 = {
#include "chaostgentrance/land/entrance_land_draw_58.inc"
};
static Sint16 entrance_land_draw_59_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_59_polygons.inc"
};
static Sint32 entrance_land_draw_59_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_59_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_59_model = {
#include "chaostgentrance/land/entrance_land_draw_59_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_59 = {
#include "chaostgentrance/land/entrance_land_draw_59.inc"
};
static Sint16 entrance_land_draw_60_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_60_polygons.inc"
};
static Sint32 entrance_land_draw_60_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_60_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_60_model = {
#include "chaostgentrance/land/entrance_land_draw_60_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_60 = {
#include "chaostgentrance/land/entrance_land_draw_60.inc"
};
static Sint16 entrance_land_draw_61_polygons[81] = {
#include "chaostgentrance/land/entrance_land_draw_61_polygons.inc"
};
static Sint32 entrance_land_draw_61_vertices[59] = {
#include "chaostgentrance/land/entrance_land_draw_61_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_61_model = {
#include "chaostgentrance/land/entrance_land_draw_61_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_61 = {
#include "chaostgentrance/land/entrance_land_draw_61.inc"
};
static Sint16 entrance_land_draw_62_polygons[87] = {
#include "chaostgentrance/land/entrance_land_draw_62_polygons.inc"
};
static Sint32 entrance_land_draw_62_vertices[83] = {
#include "chaostgentrance/land/entrance_land_draw_62_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_62_model = {
#include "chaostgentrance/land/entrance_land_draw_62_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_62 = {
#include "chaostgentrance/land/entrance_land_draw_62.inc"
};
static Sint16 entrance_land_draw_63_polygons[475] = {
#include "chaostgentrance/land/entrance_land_draw_63_polygons.inc"
};
static Sint32 entrance_land_draw_63_vertices[507] = {
#include "chaostgentrance/land/entrance_land_draw_63_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_63_model = {
#include "chaostgentrance/land/entrance_land_draw_63_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_63 = {
#include "chaostgentrance/land/entrance_land_draw_63.inc"
};
static Sint16 entrance_land_draw_64_polygons[475] = {
#include "chaostgentrance/land/entrance_land_draw_64_polygons.inc"
};
static Sint32 entrance_land_draw_64_vertices[507] = {
#include "chaostgentrance/land/entrance_land_draw_64_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_64_model = {
#include "chaostgentrance/land/entrance_land_draw_64_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_64 = {
#include "chaostgentrance/land/entrance_land_draw_64.inc"
};
static Sint16 entrance_land_draw_65_polygons[103] = {
#include "chaostgentrance/land/entrance_land_draw_65_polygons.inc"
};
static Sint32 entrance_land_draw_65_vertices[115] = {
#include "chaostgentrance/land/entrance_land_draw_65_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_65_model = {
#include "chaostgentrance/land/entrance_land_draw_65_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_65 = {
#include "chaostgentrance/land/entrance_land_draw_65.inc"
};
static Sint16 entrance_land_draw_70_polygons[1583] = {
#include "chaostgentrance/land/entrance_land_draw_70_polygons.inc"
};
static Sint32 entrance_land_draw_70_vertices[1515] = {
#include "chaostgentrance/land/entrance_land_draw_70_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_70_model = {
#include "chaostgentrance/land/entrance_land_draw_70_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_70 = {
#include "chaostgentrance/land/entrance_land_draw_70.inc"
};
static Sint16 entrance_land_draw_71_polygons[613] = {
#include "chaostgentrance/land/entrance_land_draw_71_polygons.inc"
};
static Sint32 entrance_land_draw_71_vertices[591] = {
#include "chaostgentrance/land/entrance_land_draw_71_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_71_model = {
#include "chaostgentrance/land/entrance_land_draw_71_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_71 = {
#include "chaostgentrance/land/entrance_land_draw_71.inc"
};
static Sint16 entrance_land_draw_72_polygons[851] = {
#include "chaostgentrance/land/entrance_land_draw_72_polygons.inc"
};
static Sint32 entrance_land_draw_72_vertices[811] = {
#include "chaostgentrance/land/entrance_land_draw_72_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_72_model = {
#include "chaostgentrance/land/entrance_land_draw_72_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_72 = {
#include "chaostgentrance/land/entrance_land_draw_72.inc"
};
static Sint16 entrance_land_draw_73_polygons[613] = {
#include "chaostgentrance/land/entrance_land_draw_73_polygons.inc"
};
static Sint32 entrance_land_draw_73_vertices[591] = {
#include "chaostgentrance/land/entrance_land_draw_73_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_73_model = {
#include "chaostgentrance/land/entrance_land_draw_73_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_73 = {
#include "chaostgentrance/land/entrance_land_draw_73.inc"
};
static Sint16 entrance_land_draw_74_polygons[851] = {
#include "chaostgentrance/land/entrance_land_draw_74_polygons.inc"
};
static Sint32 entrance_land_draw_74_vertices[811] = {
#include "chaostgentrance/land/entrance_land_draw_74_vertices.inc"
};
static NJS_CNK_MODEL entrance_land_draw_74_model = {
#include "chaostgentrance/land/entrance_land_draw_74_model.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_74 = {
#include "chaostgentrance/land/entrance_land_draw_74.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_0 = {
#include "chaostgentrance/land/entrance_land_draw_0.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_15 = {
#include "chaostgentrance/land/entrance_land_draw_15.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_16 = {
#include "chaostgentrance/land/entrance_land_draw_16.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_18 = {
#include "chaostgentrance/land/entrance_land_draw_18.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_47 = {
#include "chaostgentrance/land/entrance_land_draw_47.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_48 = {
#include "chaostgentrance/land/entrance_land_draw_48.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_49 = {
#include "chaostgentrance/land/entrance_land_draw_49.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_50 = {
#include "chaostgentrance/land/entrance_land_draw_50.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_51 = {
#include "chaostgentrance/land/entrance_land_draw_51.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_52 = {
#include "chaostgentrance/land/entrance_land_draw_52.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_53 = {
#include "chaostgentrance/land/entrance_land_draw_53.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_54 = {
#include "chaostgentrance/land/entrance_land_draw_54.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_55 = {
#include "chaostgentrance/land/entrance_land_draw_55.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_66 = {
#include "chaostgentrance/land/entrance_land_draw_66.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_67 = {
#include "chaostgentrance/land/entrance_land_draw_67.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_68 = {
#include "chaostgentrance/land/entrance_land_draw_68.inc"
};
static NJS_CNK_OBJECT entrance_land_draw_69 = {
#include "chaostgentrance/land/entrance_land_draw_69.inc"
};
static struct land_entry entrance_land_entries[75] = {
#include "chaostgentrance/land/entrance_land_entries.inc"
};
struct land_table entrance_land = {
#include "chaostgentrance/land/entrance_land.inc"
};
