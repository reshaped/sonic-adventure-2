#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg32/sky.h"

/* the file names that meteor_herd_changing_land and its texture list point at.
 * each starts on a 4-byte boundary */
const char meteor_herd_changing_land_filenames[16] = {
#include "stg32/sky_changes/meteor_herd_changing_land_filenames.inc"
};

/* STG32.PRS's texture lists, chunk models and background land, from the
 * disc. its D lies between terrain.c and sky_surfaces.c */
static NJS_TEXNAME meteor_herd_changing_land_textures[1] = {
#include "stg32/sky_changes/meteor_herd_changing_land_textures.inc"
};
NJS_TEXLIST meteor_herd_changing_surface_texlist = {
#include "stg32/sky_changes/meteor_herd_changing_surface_texlist.inc"
};
static Sint16 meteor_herd_changing_land_draw_47_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_47_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_47_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_47_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_47_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_47_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_50 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_50.inc"
};
static Sint16 meteor_herd_changing_land_draw_46_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_46_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_46_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_46_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_46_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_46_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_46 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_46.inc"
};
static Sint16 meteor_herd_changing_land_draw_45_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_45_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_45_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_45_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_45_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_45_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_45 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_45.inc"
};
static Sint16 meteor_herd_changing_land_draw_44_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_44_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_44_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_44_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_44_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_44_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_44 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_44.inc"
};
static Sint16 meteor_herd_changing_land_draw_43_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_43_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_43_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_43_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_43_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_43_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_43 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_43.inc"
};
static Sint16 meteor_herd_changing_land_draw_42_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_42_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_42_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_42_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_42_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_42_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_42 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_42.inc"
};
static Sint16 meteor_herd_changing_land_draw_41_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_41_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_41_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_41_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_41_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_41_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_41 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_41.inc"
};
static Sint16 meteor_herd_changing_land_draw_40_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_40_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_40_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_40_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_40_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_40_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_40 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_40.inc"
};
static Sint16 meteor_herd_changing_land_draw_39_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_39_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_39_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_39_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_39_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_39_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_39 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_39.inc"
};
static Sint16 meteor_herd_changing_land_draw_38_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_38_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_38_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_38_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_38_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_38_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_38 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_38.inc"
};
static Sint16 meteor_herd_changing_land_draw_37_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_37_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_37_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_37_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_37_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_37_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_37 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_37.inc"
};
static Sint16 meteor_herd_changing_land_draw_36_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_36_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_36_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_36_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_36_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_36_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_36 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_36.inc"
};
static Sint16 meteor_herd_changing_land_draw_35_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_35_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_35_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_35_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_35_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_35_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_35 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_35.inc"
};
static Sint16 meteor_herd_changing_land_draw_34_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_34_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_34_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_34_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_34_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_34_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_34 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_34.inc"
};
static Sint16 meteor_herd_changing_land_draw_33_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_33_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_33_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_33_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_33_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_33_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_33 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_33.inc"
};
static Sint16 meteor_herd_changing_land_draw_32_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_32_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_32_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_32_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_32_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_32_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_32 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_32.inc"
};
static Sint16 meteor_herd_changing_land_draw_31_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_31_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_31_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_31_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_31_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_31_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_31 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_31.inc"
};
static Sint16 meteor_herd_changing_land_draw_30_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_30_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_30_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_30_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_30_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_30_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_30 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_30.inc"
};
static Sint16 meteor_herd_changing_land_draw_29_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_29_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_29_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_29_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_29_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_29_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_29 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_29.inc"
};
static Sint16 meteor_herd_changing_land_draw_28_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_28_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_28_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_28_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_28_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_28_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_28 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_28.inc"
};
static Sint16 meteor_herd_changing_land_draw_27_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_27_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_27_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_27_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_27_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_27_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_27 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_27.inc"
};
static Sint16 meteor_herd_changing_land_draw_26_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_26_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_26_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_26_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_26_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_26_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_26 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_26.inc"
};
static Sint16 meteor_herd_changing_land_draw_25_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_25_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_25_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_25_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_25_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_25_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_25 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_25.inc"
};
static Sint16 meteor_herd_changing_land_draw_24_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_24_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_24_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_24_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_24_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_24_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_24 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_24.inc"
};
static Sint16 meteor_herd_changing_land_draw_23_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_23_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_23_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_23_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_23_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_23_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_23 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_23.inc"
};
static Sint16 meteor_herd_changing_land_draw_22_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_22_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_22_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_22_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_22_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_22_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_22 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_22.inc"
};
static Sint16 meteor_herd_changing_land_draw_21_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_21_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_21_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_21_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_21_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_21_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_21 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_21.inc"
};
static Sint16 meteor_herd_changing_land_draw_20_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_20_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_20_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_20_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_20_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_20_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_20 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_20.inc"
};
static Sint16 meteor_herd_changing_land_draw_19_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_19_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_19_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_19_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_19_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_19_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_19 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_19.inc"
};
static Sint16 meteor_herd_changing_land_draw_18_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_18_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_18_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_18_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_18_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_18_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_18 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_18.inc"
};
static Sint16 meteor_herd_changing_land_draw_17_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_17_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_17_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_17_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_17_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_17_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_17 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_17.inc"
};
static Sint16 meteor_herd_changing_land_draw_16_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_16_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_16_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_16_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_16_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_16_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_16 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_16.inc"
};
static Sint16 meteor_herd_changing_land_draw_15_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_15_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_15_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_15_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_15_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_15_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_15 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_15.inc"
};
static Sint16 meteor_herd_changing_land_draw_14_polygons[31] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_14_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_14_vertices[27] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_14_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_14_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_14_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_14 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_14.inc"
};
static Sint16 meteor_herd_changing_land_draw_13_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_13_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_13_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_13_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_13_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_13_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_13 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_13.inc"
};
static Sint16 meteor_herd_changing_land_draw_12_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_12_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_12_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_12_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_12_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_12_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_12 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_12.inc"
};
static Sint16 meteor_herd_changing_land_draw_11_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_11_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_11_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_11_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_11_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_11_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_11 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_11.inc"
};
static Sint16 meteor_herd_changing_land_draw_10_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_10_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_10_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_10_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_10_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_10_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_10 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_10.inc"
};
static Sint16 meteor_herd_changing_land_draw_9_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_9_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_9_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_9_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_9_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_9_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_9 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_9.inc"
};
static Sint16 meteor_herd_changing_land_draw_8_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_8_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_8_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_8_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_8_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_8_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_8 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_8.inc"
};
static Sint16 meteor_herd_changing_land_draw_7_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_7_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_7_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_7_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_7_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_7_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_7 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_7.inc"
};
static Sint16 meteor_herd_changing_land_draw_6_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_6_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_6_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_6_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_6_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_6 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_6.inc"
};
static Sint16 meteor_herd_changing_land_draw_5_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_5_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_5_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_5_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_5_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_5 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_5.inc"
};
static Sint16 meteor_herd_changing_land_draw_4_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_4_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_4_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_4_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_4_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_4 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_4.inc"
};
static Sint16 meteor_herd_changing_land_draw_3_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_3_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_3_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_3_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_3_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_3 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_3.inc"
};
static Sint16 meteor_herd_changing_land_draw_2_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_2_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_2_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_2_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_2_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_2_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_2 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_2.inc"
};
static Sint16 meteor_herd_changing_land_draw_1_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_1_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_1_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_1_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_1_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_1 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_1.inc"
};
static Sint16 meteor_herd_changing_land_draw_0_polygons[25] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_0_polygons.inc"
};
static Sint32 meteor_herd_changing_land_draw_0_vertices[19] = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL meteor_herd_changing_land_draw_0_model = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_0 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_0.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_49 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_49.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_48 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_48.inc"
};
static NJS_CNK_OBJECT meteor_herd_changing_land_draw_47 = {
#include "stg32/sky_changes/meteor_herd_changing_land_draw_47.inc"
};
static struct land_entry meteor_herd_changing_land_entries[51] = {
#include "stg32/sky_changes/meteor_herd_changing_land_entries.inc"
};
struct land_table meteor_herd_changing_land = {
#include "stg32/sky_changes/meteor_herd_changing_land.inc"
};
