#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg18/water_surface.h"

/* These word-aligned filenames are shared by all four water surfaces. */
const char dry_lagoon_water_filenames[64] = {
#include "stg18/water_filenames/dry_lagoon_water_filenames.inc"
};

/* STG18.PRS's texture lists, chunk models, UV scrolls, chunk model pointers
 * and uv scroll pointers, from the disc. its D lies between dry_lagoon.c's
 * DROUTES and its D */
static NJS_TEXNAME background_water_names[1] = {
#include "stg18/water_filenames/background_water_names.inc"
};
NJS_TEXLIST dry_lagoon_water_surface_texlist = {
#include "stg18/water_filenames/dry_lagoon_water_surface_texlist.inc"
};
static Sint16 south_water_0_polygons[199] = {
#include "stg18/water_filenames/south_water_0_polygons.inc"
};
static Sint32 south_water_0_vertices[123] = {
#include "stg18/water_filenames/south_water_0_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_0 = {
#include "stg18/water_filenames/south_water_model_0.inc"
};
static NJS_CNK_OBJECT south_water_0_object = {
#include "stg18/water_filenames/south_water_0_object.inc"
};
static NJS_TEXNAME water_frame_1_names[1] = {
#include "stg18/water_filenames/water_frame_1_names.inc"
};
static NJS_TEXLIST water_frame_1_texlist = {
#include "stg18/water_filenames/water_frame_1_texlist.inc"
};
static Sint16 south_water_1_polygons[199] = {
#include "stg18/water_filenames/south_water_1_polygons.inc"
};
static Sint32 south_water_1_vertices[123] = {
#include "stg18/water_filenames/south_water_1_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_1 = {
#include "stg18/water_filenames/south_water_model_1.inc"
};
static NJS_CNK_OBJECT south_water_1_object = {
#include "stg18/water_filenames/south_water_1_object.inc"
};
static NJS_TEXNAME water_frame_2_names[1] = {
#include "stg18/water_filenames/water_frame_2_names.inc"
};
static NJS_TEXLIST water_frame_2_texlist = {
#include "stg18/water_filenames/water_frame_2_texlist.inc"
};
static Sint16 south_water_2_polygons[199] = {
#include "stg18/water_filenames/south_water_2_polygons.inc"
};
static Sint32 south_water_2_vertices[123] = {
#include "stg18/water_filenames/south_water_2_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_2 = {
#include "stg18/water_filenames/south_water_model_2.inc"
};
static NJS_CNK_OBJECT south_water_2_object = {
#include "stg18/water_filenames/south_water_2_object.inc"
};
static NJS_TEXNAME water_frame_3_names[1] = {
#include "stg18/water_filenames/water_frame_3_names.inc"
};
static NJS_TEXLIST water_frame_3_texlist = {
#include "stg18/water_filenames/water_frame_3_texlist.inc"
};
static Sint16 south_water_3_polygons[199] = {
#include "stg18/water_filenames/south_water_3_polygons.inc"
};
static Sint32 south_water_3_vertices[123] = {
#include "stg18/water_filenames/south_water_3_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_3 = {
#include "stg18/water_filenames/south_water_model_3.inc"
};
static NJS_CNK_OBJECT south_water_3_object = {
#include "stg18/water_filenames/south_water_3_object.inc"
};
static NJS_TEXNAME water_frame_4_names[1] = {
#include "stg18/water_filenames/water_frame_4_names.inc"
};
static NJS_TEXLIST water_frame_4_texlist = {
#include "stg18/water_filenames/water_frame_4_texlist.inc"
};
static Sint16 south_water_4_polygons[199] = {
#include "stg18/water_filenames/south_water_4_polygons.inc"
};
static Sint32 south_water_4_vertices[123] = {
#include "stg18/water_filenames/south_water_4_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_4 = {
#include "stg18/water_filenames/south_water_model_4.inc"
};
static NJS_CNK_OBJECT south_water_4_object = {
#include "stg18/water_filenames/south_water_4_object.inc"
};
static NJS_TEXNAME water_frame_5_names[1] = {
#include "stg18/water_filenames/water_frame_5_names.inc"
};
static NJS_TEXLIST water_frame_5_texlist = {
#include "stg18/water_filenames/water_frame_5_texlist.inc"
};
static Sint16 south_water_5_polygons[199] = {
#include "stg18/water_filenames/south_water_5_polygons.inc"
};
static Sint32 south_water_5_vertices[123] = {
#include "stg18/water_filenames/south_water_5_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_5 = {
#include "stg18/water_filenames/south_water_model_5.inc"
};
static NJS_CNK_OBJECT south_water_5_object = {
#include "stg18/water_filenames/south_water_5_object.inc"
};
static NJS_TEXNAME water_frame_6_names[1] = {
#include "stg18/water_filenames/water_frame_6_names.inc"
};
static NJS_TEXLIST water_frame_6_texlist = {
#include "stg18/water_filenames/water_frame_6_texlist.inc"
};
static Sint16 south_water_6_polygons[199] = {
#include "stg18/water_filenames/south_water_6_polygons.inc"
};
static Sint32 south_water_6_vertices[123] = {
#include "stg18/water_filenames/south_water_6_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_6 = {
#include "stg18/water_filenames/south_water_model_6.inc"
};
static NJS_CNK_OBJECT south_water_6_object = {
#include "stg18/water_filenames/south_water_6_object.inc"
};
static NJS_TEXNAME water_frame_7_names[1] = {
#include "stg18/water_filenames/water_frame_7_names.inc"
};
static NJS_TEXLIST water_frame_7_texlist = {
#include "stg18/water_filenames/water_frame_7_texlist.inc"
};
static Sint16 south_water_7_polygons[199] = {
#include "stg18/water_filenames/south_water_7_polygons.inc"
};
static Sint32 south_water_7_vertices[123] = {
#include "stg18/water_filenames/south_water_7_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_7 = {
#include "stg18/water_filenames/south_water_model_7.inc"
};
static NJS_CNK_OBJECT south_water_7_object = {
#include "stg18/water_filenames/south_water_7_object.inc"
};
static NJS_TEXNAME water_frame_8_names[1] = {
#include "stg18/water_filenames/water_frame_8_names.inc"
};
static NJS_TEXLIST water_frame_8_texlist = {
#include "stg18/water_filenames/water_frame_8_texlist.inc"
};
static Sint16 south_water_8_polygons[199] = {
#include "stg18/water_filenames/south_water_8_polygons.inc"
};
static Sint32 south_water_8_vertices[123] = {
#include "stg18/water_filenames/south_water_8_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_8 = {
#include "stg18/water_filenames/south_water_model_8.inc"
};
static NJS_CNK_OBJECT south_water_8_object = {
#include "stg18/water_filenames/south_water_8_object.inc"
};
static NJS_TEXNAME water_frame_9_names[1] = {
#include "stg18/water_filenames/water_frame_9_names.inc"
};
static NJS_TEXLIST water_frame_9_texlist = {
#include "stg18/water_filenames/water_frame_9_texlist.inc"
};
static Sint16 south_water_9_polygons[199] = {
#include "stg18/water_filenames/south_water_9_polygons.inc"
};
static Sint32 south_water_9_vertices[123] = {
#include "stg18/water_filenames/south_water_9_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_9 = {
#include "stg18/water_filenames/south_water_model_9.inc"
};
static NJS_CNK_OBJECT south_water_9_object = {
#include "stg18/water_filenames/south_water_9_object.inc"
};
static NJS_TEXNAME water_frame_10_names[1] = {
#include "stg18/water_filenames/water_frame_10_names.inc"
};
static NJS_TEXLIST water_frame_10_texlist = {
#include "stg18/water_filenames/water_frame_10_texlist.inc"
};
static Sint16 south_water_10_polygons[199] = {
#include "stg18/water_filenames/south_water_10_polygons.inc"
};
static Sint32 south_water_10_vertices[123] = {
#include "stg18/water_filenames/south_water_10_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_10 = {
#include "stg18/water_filenames/south_water_model_10.inc"
};
static NJS_CNK_OBJECT south_water_10_object = {
#include "stg18/water_filenames/south_water_10_object.inc"
};
static NJS_TEXNAME water_frame_11_names[1] = {
#include "stg18/water_filenames/water_frame_11_names.inc"
};
static NJS_TEXLIST water_frame_11_texlist = {
#include "stg18/water_filenames/water_frame_11_texlist.inc"
};
static Sint16 south_water_11_polygons[199] = {
#include "stg18/water_filenames/south_water_11_polygons.inc"
};
static Sint32 south_water_11_vertices[123] = {
#include "stg18/water_filenames/south_water_11_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_11 = {
#include "stg18/water_filenames/south_water_model_11.inc"
};
static NJS_CNK_OBJECT south_water_11_object = {
#include "stg18/water_filenames/south_water_11_object.inc"
};
static NJS_TEXNAME water_frame_12_names[1] = {
#include "stg18/water_filenames/water_frame_12_names.inc"
};
static NJS_TEXLIST water_frame_12_texlist = {
#include "stg18/water_filenames/water_frame_12_texlist.inc"
};
static Sint16 south_water_12_polygons[199] = {
#include "stg18/water_filenames/south_water_12_polygons.inc"
};
static Sint32 south_water_12_vertices[123] = {
#include "stg18/water_filenames/south_water_12_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_12 = {
#include "stg18/water_filenames/south_water_model_12.inc"
};
static NJS_CNK_OBJECT south_water_12_object = {
#include "stg18/water_filenames/south_water_12_object.inc"
};
static NJS_TEXNAME water_frame_13_names[1] = {
#include "stg18/water_filenames/water_frame_13_names.inc"
};
static NJS_TEXLIST water_frame_13_texlist = {
#include "stg18/water_filenames/water_frame_13_texlist.inc"
};
static Sint16 south_water_13_polygons[199] = {
#include "stg18/water_filenames/south_water_13_polygons.inc"
};
static Sint32 south_water_13_vertices[123] = {
#include "stg18/water_filenames/south_water_13_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_13 = {
#include "stg18/water_filenames/south_water_model_13.inc"
};
static NJS_CNK_OBJECT south_water_13_object = {
#include "stg18/water_filenames/south_water_13_object.inc"
};
static NJS_TEXNAME water_frame_14_names[1] = {
#include "stg18/water_filenames/water_frame_14_names.inc"
};
static NJS_TEXLIST water_frame_14_texlist = {
#include "stg18/water_filenames/water_frame_14_texlist.inc"
};
static Sint16 south_water_14_polygons[199] = {
#include "stg18/water_filenames/south_water_14_polygons.inc"
};
static Sint32 south_water_14_vertices[123] = {
#include "stg18/water_filenames/south_water_14_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_14 = {
#include "stg18/water_filenames/south_water_model_14.inc"
};
static NJS_CNK_OBJECT south_water_14_object = {
#include "stg18/water_filenames/south_water_14_object.inc"
};
static NJS_TEXNAME water_frame_15_names[1] = {
#include "stg18/water_filenames/water_frame_15_names.inc"
};
static NJS_TEXLIST water_frame_15_texlist = {
#include "stg18/water_filenames/water_frame_15_texlist.inc"
};
static Sint16 south_water_15_polygons[199] = {
#include "stg18/water_filenames/south_water_15_polygons.inc"
};
static Sint32 south_water_15_vertices[123] = {
#include "stg18/water_filenames/south_water_15_vertices.inc"
};
static NJS_CNK_MODEL south_water_model_15 = {
#include "stg18/water_filenames/south_water_model_15.inc"
};
static NJS_CNK_OBJECT south_water_15_object = {
#include "stg18/water_filenames/south_water_15_object.inc"
};
static NJS_TEXNAME water_frame_16_names[1] = {
#include "stg18/water_filenames/water_frame_16_names.inc"
};
static NJS_TEXLIST water_frame_16_texlist = {
#include "stg18/water_filenames/water_frame_16_texlist.inc"
};
static Sint16 north_water_0_polygons[199] = {
#include "stg18/water_filenames/north_water_0_polygons.inc"
};
static Sint32 north_water_0_vertices[123] = {
#include "stg18/water_filenames/north_water_0_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_0 = {
#include "stg18/water_filenames/north_water_model_0.inc"
};
static NJS_CNK_OBJECT north_water_0_object = {
#include "stg18/water_filenames/north_water_0_object.inc"
};
static NJS_TEXNAME water_frame_17_names[1] = {
#include "stg18/water_filenames/water_frame_17_names.inc"
};
static NJS_TEXLIST water_frame_17_texlist = {
#include "stg18/water_filenames/water_frame_17_texlist.inc"
};
static Sint16 north_water_1_polygons[199] = {
#include "stg18/water_filenames/north_water_1_polygons.inc"
};
static Sint32 north_water_1_vertices[123] = {
#include "stg18/water_filenames/north_water_1_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_1 = {
#include "stg18/water_filenames/north_water_model_1.inc"
};
static NJS_CNK_OBJECT north_water_1_object = {
#include "stg18/water_filenames/north_water_1_object.inc"
};
static NJS_TEXNAME water_frame_18_names[1] = {
#include "stg18/water_filenames/water_frame_18_names.inc"
};
static NJS_TEXLIST water_frame_18_texlist = {
#include "stg18/water_filenames/water_frame_18_texlist.inc"
};
static Sint16 north_water_2_polygons[199] = {
#include "stg18/water_filenames/north_water_2_polygons.inc"
};
static Sint32 north_water_2_vertices[123] = {
#include "stg18/water_filenames/north_water_2_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_2 = {
#include "stg18/water_filenames/north_water_model_2.inc"
};
static NJS_CNK_OBJECT north_water_2_object = {
#include "stg18/water_filenames/north_water_2_object.inc"
};
static NJS_TEXNAME water_frame_19_names[1] = {
#include "stg18/water_filenames/water_frame_19_names.inc"
};
static NJS_TEXLIST water_frame_19_texlist = {
#include "stg18/water_filenames/water_frame_19_texlist.inc"
};
static Sint16 north_water_3_polygons[199] = {
#include "stg18/water_filenames/north_water_3_polygons.inc"
};
static Sint32 north_water_3_vertices[123] = {
#include "stg18/water_filenames/north_water_3_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_3 = {
#include "stg18/water_filenames/north_water_model_3.inc"
};
static NJS_CNK_OBJECT north_water_3_object = {
#include "stg18/water_filenames/north_water_3_object.inc"
};
static NJS_TEXNAME water_frame_20_names[1] = {
#include "stg18/water_filenames/water_frame_20_names.inc"
};
static NJS_TEXLIST water_frame_20_texlist = {
#include "stg18/water_filenames/water_frame_20_texlist.inc"
};
static Sint16 north_water_4_polygons[199] = {
#include "stg18/water_filenames/north_water_4_polygons.inc"
};
static Sint32 north_water_4_vertices[123] = {
#include "stg18/water_filenames/north_water_4_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_4 = {
#include "stg18/water_filenames/north_water_model_4.inc"
};
static NJS_CNK_OBJECT north_water_4_object = {
#include "stg18/water_filenames/north_water_4_object.inc"
};
static NJS_TEXNAME water_frame_21_names[1] = {
#include "stg18/water_filenames/water_frame_21_names.inc"
};
static NJS_TEXLIST water_frame_21_texlist = {
#include "stg18/water_filenames/water_frame_21_texlist.inc"
};
static Sint16 north_water_5_polygons[199] = {
#include "stg18/water_filenames/north_water_5_polygons.inc"
};
static Sint32 north_water_5_vertices[123] = {
#include "stg18/water_filenames/north_water_5_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_5 = {
#include "stg18/water_filenames/north_water_model_5.inc"
};
static NJS_CNK_OBJECT north_water_5_object = {
#include "stg18/water_filenames/north_water_5_object.inc"
};
static NJS_TEXNAME water_frame_22_names[1] = {
#include "stg18/water_filenames/water_frame_22_names.inc"
};
static NJS_TEXLIST water_frame_22_texlist = {
#include "stg18/water_filenames/water_frame_22_texlist.inc"
};
static Sint16 north_water_6_polygons[199] = {
#include "stg18/water_filenames/north_water_6_polygons.inc"
};
static Sint32 north_water_6_vertices[123] = {
#include "stg18/water_filenames/north_water_6_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_6 = {
#include "stg18/water_filenames/north_water_model_6.inc"
};
static NJS_CNK_OBJECT north_water_6_object = {
#include "stg18/water_filenames/north_water_6_object.inc"
};
static NJS_TEXNAME water_frame_23_names[1] = {
#include "stg18/water_filenames/water_frame_23_names.inc"
};
static NJS_TEXLIST water_frame_23_texlist = {
#include "stg18/water_filenames/water_frame_23_texlist.inc"
};
static Sint16 north_water_7_polygons[199] = {
#include "stg18/water_filenames/north_water_7_polygons.inc"
};
static Sint32 north_water_7_vertices[123] = {
#include "stg18/water_filenames/north_water_7_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_7 = {
#include "stg18/water_filenames/north_water_model_7.inc"
};
static NJS_CNK_OBJECT north_water_7_object = {
#include "stg18/water_filenames/north_water_7_object.inc"
};
static NJS_TEXNAME water_frame_24_names[1] = {
#include "stg18/water_filenames/water_frame_24_names.inc"
};
static NJS_TEXLIST water_frame_24_texlist = {
#include "stg18/water_filenames/water_frame_24_texlist.inc"
};
static Sint16 north_water_8_polygons[199] = {
#include "stg18/water_filenames/north_water_8_polygons.inc"
};
static Sint32 north_water_8_vertices[123] = {
#include "stg18/water_filenames/north_water_8_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_8 = {
#include "stg18/water_filenames/north_water_model_8.inc"
};
static NJS_CNK_OBJECT north_water_8_object = {
#include "stg18/water_filenames/north_water_8_object.inc"
};
static NJS_TEXNAME water_frame_25_names[1] = {
#include "stg18/water_filenames/water_frame_25_names.inc"
};
static NJS_TEXLIST water_frame_25_texlist = {
#include "stg18/water_filenames/water_frame_25_texlist.inc"
};
static Sint16 north_water_9_polygons[199] = {
#include "stg18/water_filenames/north_water_9_polygons.inc"
};
static Sint32 north_water_9_vertices[123] = {
#include "stg18/water_filenames/north_water_9_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_9 = {
#include "stg18/water_filenames/north_water_model_9.inc"
};
static NJS_CNK_OBJECT north_water_9_object = {
#include "stg18/water_filenames/north_water_9_object.inc"
};
static NJS_TEXNAME water_frame_26_names[1] = {
#include "stg18/water_filenames/water_frame_26_names.inc"
};
static NJS_TEXLIST water_frame_26_texlist = {
#include "stg18/water_filenames/water_frame_26_texlist.inc"
};
static Sint16 north_water_10_polygons[199] = {
#include "stg18/water_filenames/north_water_10_polygons.inc"
};
static Sint32 north_water_10_vertices[123] = {
#include "stg18/water_filenames/north_water_10_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_10 = {
#include "stg18/water_filenames/north_water_model_10.inc"
};
static NJS_CNK_OBJECT north_water_10_object = {
#include "stg18/water_filenames/north_water_10_object.inc"
};
static NJS_TEXNAME water_frame_27_names[1] = {
#include "stg18/water_filenames/water_frame_27_names.inc"
};
static NJS_TEXLIST water_frame_27_texlist = {
#include "stg18/water_filenames/water_frame_27_texlist.inc"
};
static Sint16 north_water_11_polygons[199] = {
#include "stg18/water_filenames/north_water_11_polygons.inc"
};
static Sint32 north_water_11_vertices[123] = {
#include "stg18/water_filenames/north_water_11_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_11 = {
#include "stg18/water_filenames/north_water_model_11.inc"
};
static NJS_CNK_OBJECT north_water_11_object = {
#include "stg18/water_filenames/north_water_11_object.inc"
};
static NJS_TEXNAME water_frame_28_names[1] = {
#include "stg18/water_filenames/water_frame_28_names.inc"
};
static NJS_TEXLIST water_frame_28_texlist = {
#include "stg18/water_filenames/water_frame_28_texlist.inc"
};
static Sint16 north_water_12_polygons[199] = {
#include "stg18/water_filenames/north_water_12_polygons.inc"
};
static Sint32 north_water_12_vertices[123] = {
#include "stg18/water_filenames/north_water_12_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_12 = {
#include "stg18/water_filenames/north_water_model_12.inc"
};
static NJS_CNK_OBJECT north_water_12_object = {
#include "stg18/water_filenames/north_water_12_object.inc"
};
static NJS_TEXNAME water_frame_29_names[1] = {
#include "stg18/water_filenames/water_frame_29_names.inc"
};
static NJS_TEXLIST water_frame_29_texlist = {
#include "stg18/water_filenames/water_frame_29_texlist.inc"
};
static Sint16 north_water_13_polygons[199] = {
#include "stg18/water_filenames/north_water_13_polygons.inc"
};
static Sint32 north_water_13_vertices[123] = {
#include "stg18/water_filenames/north_water_13_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_13 = {
#include "stg18/water_filenames/north_water_model_13.inc"
};
static NJS_CNK_OBJECT north_water_13_object = {
#include "stg18/water_filenames/north_water_13_object.inc"
};
static NJS_TEXNAME water_frame_30_names[1] = {
#include "stg18/water_filenames/water_frame_30_names.inc"
};
static NJS_TEXLIST water_frame_30_texlist = {
#include "stg18/water_filenames/water_frame_30_texlist.inc"
};
static Sint16 north_water_14_polygons[199] = {
#include "stg18/water_filenames/north_water_14_polygons.inc"
};
static Sint32 north_water_14_vertices[123] = {
#include "stg18/water_filenames/north_water_14_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_14 = {
#include "stg18/water_filenames/north_water_model_14.inc"
};
static NJS_CNK_OBJECT north_water_14_object = {
#include "stg18/water_filenames/north_water_14_object.inc"
};
static NJS_TEXNAME water_frame_31_names[1] = {
#include "stg18/water_filenames/water_frame_31_names.inc"
};
static NJS_TEXLIST water_frame_31_texlist = {
#include "stg18/water_filenames/water_frame_31_texlist.inc"
};
static Sint16 north_water_15_polygons[199] = {
#include "stg18/water_filenames/north_water_15_polygons.inc"
};
static Sint32 north_water_15_vertices[123] = {
#include "stg18/water_filenames/north_water_15_vertices.inc"
};
static NJS_CNK_MODEL north_water_model_15 = {
#include "stg18/water_filenames/north_water_model_15.inc"
};
static NJS_CNK_OBJECT north_water_15_object = {
#include "stg18/water_filenames/north_water_15_object.inc"
};
static NJS_TEXNAME water_frame_32_names[1] = {
#include "stg18/water_filenames/water_frame_32_names.inc"
};
static NJS_TEXLIST water_frame_32_texlist = {
#include "stg18/water_filenames/water_frame_32_texlist.inc"
};
static Sint16 middle_water_0_polygons[683] = {
#include "stg18/water_filenames/middle_water_0_polygons.inc"
};
static Sint32 middle_water_0_vertices[311] = {
#include "stg18/water_filenames/middle_water_0_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_0 = {
#include "stg18/water_filenames/middle_water_model_0.inc"
};
static NJS_CNK_OBJECT middle_water_0_object = {
#include "stg18/water_filenames/middle_water_0_object.inc"
};
static NJS_TEXNAME water_frame_33_names[1] = {
#include "stg18/water_filenames/water_frame_33_names.inc"
};
static NJS_TEXLIST water_frame_33_texlist = {
#include "stg18/water_filenames/water_frame_33_texlist.inc"
};
static Sint16 middle_water_1_polygons[683] = {
#include "stg18/water_filenames/middle_water_1_polygons.inc"
};
static Sint32 middle_water_1_vertices[311] = {
#include "stg18/water_filenames/middle_water_1_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_1 = {
#include "stg18/water_filenames/middle_water_model_1.inc"
};
static NJS_CNK_OBJECT middle_water_1_object = {
#include "stg18/water_filenames/middle_water_1_object.inc"
};
static NJS_TEXNAME water_frame_34_names[1] = {
#include "stg18/water_filenames/water_frame_34_names.inc"
};
static NJS_TEXLIST water_frame_34_texlist = {
#include "stg18/water_filenames/water_frame_34_texlist.inc"
};
static Sint16 middle_water_2_polygons[683] = {
#include "stg18/water_filenames/middle_water_2_polygons.inc"
};
static Sint32 middle_water_2_vertices[311] = {
#include "stg18/water_filenames/middle_water_2_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_2 = {
#include "stg18/water_filenames/middle_water_model_2.inc"
};
static NJS_CNK_OBJECT middle_water_2_object = {
#include "stg18/water_filenames/middle_water_2_object.inc"
};
static NJS_TEXNAME water_frame_35_names[1] = {
#include "stg18/water_filenames/water_frame_35_names.inc"
};
static NJS_TEXLIST water_frame_35_texlist = {
#include "stg18/water_filenames/water_frame_35_texlist.inc"
};
static Sint16 middle_water_3_polygons[683] = {
#include "stg18/water_filenames/middle_water_3_polygons.inc"
};
static Sint32 middle_water_3_vertices[311] = {
#include "stg18/water_filenames/middle_water_3_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_3 = {
#include "stg18/water_filenames/middle_water_model_3.inc"
};
static NJS_CNK_OBJECT middle_water_3_object = {
#include "stg18/water_filenames/middle_water_3_object.inc"
};
static NJS_TEXNAME water_frame_36_names[1] = {
#include "stg18/water_filenames/water_frame_36_names.inc"
};
static NJS_TEXLIST water_frame_36_texlist = {
#include "stg18/water_filenames/water_frame_36_texlist.inc"
};
static Sint16 middle_water_4_polygons[683] = {
#include "stg18/water_filenames/middle_water_4_polygons.inc"
};
static Sint32 middle_water_4_vertices[311] = {
#include "stg18/water_filenames/middle_water_4_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_4 = {
#include "stg18/water_filenames/middle_water_model_4.inc"
};
static NJS_CNK_OBJECT middle_water_4_object = {
#include "stg18/water_filenames/middle_water_4_object.inc"
};
static NJS_TEXNAME water_frame_37_names[1] = {
#include "stg18/water_filenames/water_frame_37_names.inc"
};
static NJS_TEXLIST water_frame_37_texlist = {
#include "stg18/water_filenames/water_frame_37_texlist.inc"
};
static Sint16 middle_water_5_polygons[683] = {
#include "stg18/water_filenames/middle_water_5_polygons.inc"
};
static Sint32 middle_water_5_vertices[311] = {
#include "stg18/water_filenames/middle_water_5_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_5 = {
#include "stg18/water_filenames/middle_water_model_5.inc"
};
static NJS_CNK_OBJECT middle_water_5_object = {
#include "stg18/water_filenames/middle_water_5_object.inc"
};
static NJS_TEXNAME water_frame_38_names[1] = {
#include "stg18/water_filenames/water_frame_38_names.inc"
};
static NJS_TEXLIST water_frame_38_texlist = {
#include "stg18/water_filenames/water_frame_38_texlist.inc"
};
static Sint16 middle_water_6_polygons[683] = {
#include "stg18/water_filenames/middle_water_6_polygons.inc"
};
static Sint32 middle_water_6_vertices[311] = {
#include "stg18/water_filenames/middle_water_6_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_6 = {
#include "stg18/water_filenames/middle_water_model_6.inc"
};
static NJS_CNK_OBJECT middle_water_6_object = {
#include "stg18/water_filenames/middle_water_6_object.inc"
};
static NJS_TEXNAME water_frame_39_names[1] = {
#include "stg18/water_filenames/water_frame_39_names.inc"
};
static NJS_TEXLIST water_frame_39_texlist = {
#include "stg18/water_filenames/water_frame_39_texlist.inc"
};
static Sint16 middle_water_7_polygons[683] = {
#include "stg18/water_filenames/middle_water_7_polygons.inc"
};
static Sint32 middle_water_7_vertices[311] = {
#include "stg18/water_filenames/middle_water_7_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_7 = {
#include "stg18/water_filenames/middle_water_model_7.inc"
};
static NJS_CNK_OBJECT middle_water_7_object = {
#include "stg18/water_filenames/middle_water_7_object.inc"
};
static NJS_TEXNAME water_frame_40_names[1] = {
#include "stg18/water_filenames/water_frame_40_names.inc"
};
static NJS_TEXLIST water_frame_40_texlist = {
#include "stg18/water_filenames/water_frame_40_texlist.inc"
};
static Sint16 middle_water_8_polygons[683] = {
#include "stg18/water_filenames/middle_water_8_polygons.inc"
};
static Sint32 middle_water_8_vertices[311] = {
#include "stg18/water_filenames/middle_water_8_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_8 = {
#include "stg18/water_filenames/middle_water_model_8.inc"
};
static NJS_CNK_OBJECT middle_water_8_object = {
#include "stg18/water_filenames/middle_water_8_object.inc"
};
static NJS_TEXNAME water_frame_41_names[1] = {
#include "stg18/water_filenames/water_frame_41_names.inc"
};
static NJS_TEXLIST water_frame_41_texlist = {
#include "stg18/water_filenames/water_frame_41_texlist.inc"
};
static Sint16 middle_water_9_polygons[683] = {
#include "stg18/water_filenames/middle_water_9_polygons.inc"
};
static Sint32 middle_water_9_vertices[311] = {
#include "stg18/water_filenames/middle_water_9_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_9 = {
#include "stg18/water_filenames/middle_water_model_9.inc"
};
static NJS_CNK_OBJECT middle_water_9_object = {
#include "stg18/water_filenames/middle_water_9_object.inc"
};
static NJS_TEXNAME water_frame_42_names[1] = {
#include "stg18/water_filenames/water_frame_42_names.inc"
};
static NJS_TEXLIST water_frame_42_texlist = {
#include "stg18/water_filenames/water_frame_42_texlist.inc"
};
static Sint16 middle_water_10_polygons[683] = {
#include "stg18/water_filenames/middle_water_10_polygons.inc"
};
static Sint32 middle_water_10_vertices[311] = {
#include "stg18/water_filenames/middle_water_10_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_10 = {
#include "stg18/water_filenames/middle_water_model_10.inc"
};
static NJS_CNK_OBJECT middle_water_10_object = {
#include "stg18/water_filenames/middle_water_10_object.inc"
};
static NJS_TEXNAME water_frame_43_names[1] = {
#include "stg18/water_filenames/water_frame_43_names.inc"
};
static NJS_TEXLIST water_frame_43_texlist = {
#include "stg18/water_filenames/water_frame_43_texlist.inc"
};
static Sint16 middle_water_11_polygons[683] = {
#include "stg18/water_filenames/middle_water_11_polygons.inc"
};
static Sint32 middle_water_11_vertices[311] = {
#include "stg18/water_filenames/middle_water_11_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_11 = {
#include "stg18/water_filenames/middle_water_model_11.inc"
};
static NJS_CNK_OBJECT middle_water_11_object = {
#include "stg18/water_filenames/middle_water_11_object.inc"
};
static NJS_TEXNAME water_frame_44_names[1] = {
#include "stg18/water_filenames/water_frame_44_names.inc"
};
static NJS_TEXLIST water_frame_44_texlist = {
#include "stg18/water_filenames/water_frame_44_texlist.inc"
};
static Sint16 middle_water_12_polygons[683] = {
#include "stg18/water_filenames/middle_water_12_polygons.inc"
};
static Sint32 middle_water_12_vertices[311] = {
#include "stg18/water_filenames/middle_water_12_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_12 = {
#include "stg18/water_filenames/middle_water_model_12.inc"
};
static NJS_CNK_OBJECT middle_water_12_object = {
#include "stg18/water_filenames/middle_water_12_object.inc"
};
static NJS_TEXNAME water_frame_45_names[1] = {
#include "stg18/water_filenames/water_frame_45_names.inc"
};
static NJS_TEXLIST water_frame_45_texlist = {
#include "stg18/water_filenames/water_frame_45_texlist.inc"
};
static Sint16 middle_water_13_polygons[683] = {
#include "stg18/water_filenames/middle_water_13_polygons.inc"
};
static Sint32 middle_water_13_vertices[311] = {
#include "stg18/water_filenames/middle_water_13_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_13 = {
#include "stg18/water_filenames/middle_water_model_13.inc"
};
static NJS_CNK_OBJECT middle_water_13_object = {
#include "stg18/water_filenames/middle_water_13_object.inc"
};
static NJS_TEXNAME water_frame_46_names[1] = {
#include "stg18/water_filenames/water_frame_46_names.inc"
};
static NJS_TEXLIST water_frame_46_texlist = {
#include "stg18/water_filenames/water_frame_46_texlist.inc"
};
static Sint16 middle_water_14_polygons[683] = {
#include "stg18/water_filenames/middle_water_14_polygons.inc"
};
static Sint32 middle_water_14_vertices[311] = {
#include "stg18/water_filenames/middle_water_14_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_14 = {
#include "stg18/water_filenames/middle_water_model_14.inc"
};
static NJS_CNK_OBJECT middle_water_14_object = {
#include "stg18/water_filenames/middle_water_14_object.inc"
};
static NJS_TEXNAME water_frame_47_names[1] = {
#include "stg18/water_filenames/water_frame_47_names.inc"
};
static NJS_TEXLIST water_frame_47_texlist = {
#include "stg18/water_filenames/water_frame_47_texlist.inc"
};
static Sint16 middle_water_15_polygons[683] = {
#include "stg18/water_filenames/middle_water_15_polygons.inc"
};
static Sint32 middle_water_15_vertices[311] = {
#include "stg18/water_filenames/middle_water_15_vertices.inc"
};
static NJS_CNK_MODEL middle_water_model_15 = {
#include "stg18/water_filenames/middle_water_model_15.inc"
};
static NJS_CNK_OBJECT middle_water_15_object = {
#include "stg18/water_filenames/middle_water_15_object.inc"
};
static NJS_TEXNAME water_frame_48_names[1] = {
#include "stg18/water_filenames/water_frame_48_names.inc"
};
static NJS_TEXLIST water_frame_48_texlist = {
#include "stg18/water_filenames/water_frame_48_texlist.inc"
};
static Sint16 middle_water_2_0_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_0_polygons.inc"
};
static Sint32 middle_water_2_0_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_0_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_0 = {
#include "stg18/water_filenames/middle_water_2_model_0.inc"
};
static NJS_CNK_OBJECT middle_water_2_0_object = {
#include "stg18/water_filenames/middle_water_2_0_object.inc"
};
static NJS_TEXNAME water_frame_49_names[1] = {
#include "stg18/water_filenames/water_frame_49_names.inc"
};
static NJS_TEXLIST water_frame_49_texlist = {
#include "stg18/water_filenames/water_frame_49_texlist.inc"
};
static Sint16 middle_water_2_1_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_1_polygons.inc"
};
static Sint32 middle_water_2_1_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_1_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_1 = {
#include "stg18/water_filenames/middle_water_2_model_1.inc"
};
static NJS_CNK_OBJECT middle_water_2_1_object = {
#include "stg18/water_filenames/middle_water_2_1_object.inc"
};
static NJS_TEXNAME water_frame_50_names[1] = {
#include "stg18/water_filenames/water_frame_50_names.inc"
};
static NJS_TEXLIST water_frame_50_texlist = {
#include "stg18/water_filenames/water_frame_50_texlist.inc"
};
static Sint16 middle_water_2_2_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_2_polygons.inc"
};
static Sint32 middle_water_2_2_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_2_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_2 = {
#include "stg18/water_filenames/middle_water_2_model_2.inc"
};
static NJS_CNK_OBJECT middle_water_2_2_object = {
#include "stg18/water_filenames/middle_water_2_2_object.inc"
};
static NJS_TEXNAME water_frame_51_names[1] = {
#include "stg18/water_filenames/water_frame_51_names.inc"
};
static NJS_TEXLIST water_frame_51_texlist = {
#include "stg18/water_filenames/water_frame_51_texlist.inc"
};
static Sint16 middle_water_2_3_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_3_polygons.inc"
};
static Sint32 middle_water_2_3_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_3_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_3 = {
#include "stg18/water_filenames/middle_water_2_model_3.inc"
};
static NJS_CNK_OBJECT middle_water_2_3_object = {
#include "stg18/water_filenames/middle_water_2_3_object.inc"
};
static NJS_TEXNAME water_frame_52_names[1] = {
#include "stg18/water_filenames/water_frame_52_names.inc"
};
static NJS_TEXLIST water_frame_52_texlist = {
#include "stg18/water_filenames/water_frame_52_texlist.inc"
};
static Sint16 middle_water_2_4_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_4_polygons.inc"
};
static Sint32 middle_water_2_4_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_4_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_4 = {
#include "stg18/water_filenames/middle_water_2_model_4.inc"
};
static NJS_CNK_OBJECT middle_water_2_4_object = {
#include "stg18/water_filenames/middle_water_2_4_object.inc"
};
static NJS_TEXNAME water_frame_53_names[1] = {
#include "stg18/water_filenames/water_frame_53_names.inc"
};
static NJS_TEXLIST water_frame_53_texlist = {
#include "stg18/water_filenames/water_frame_53_texlist.inc"
};
static Sint16 middle_water_2_5_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_5_polygons.inc"
};
static Sint32 middle_water_2_5_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_5_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_5 = {
#include "stg18/water_filenames/middle_water_2_model_5.inc"
};
static NJS_CNK_OBJECT middle_water_2_5_object = {
#include "stg18/water_filenames/middle_water_2_5_object.inc"
};
static NJS_TEXNAME water_frame_54_names[1] = {
#include "stg18/water_filenames/water_frame_54_names.inc"
};
static NJS_TEXLIST water_frame_54_texlist = {
#include "stg18/water_filenames/water_frame_54_texlist.inc"
};
static Sint16 middle_water_2_6_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_6_polygons.inc"
};
static Sint32 middle_water_2_6_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_6_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_6 = {
#include "stg18/water_filenames/middle_water_2_model_6.inc"
};
static NJS_CNK_OBJECT middle_water_2_6_object = {
#include "stg18/water_filenames/middle_water_2_6_object.inc"
};
static NJS_TEXNAME water_frame_55_names[1] = {
#include "stg18/water_filenames/water_frame_55_names.inc"
};
static NJS_TEXLIST water_frame_55_texlist = {
#include "stg18/water_filenames/water_frame_55_texlist.inc"
};
static Sint16 middle_water_2_7_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_7_polygons.inc"
};
static Sint32 middle_water_2_7_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_7_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_7 = {
#include "stg18/water_filenames/middle_water_2_model_7.inc"
};
static NJS_CNK_OBJECT middle_water_2_7_object = {
#include "stg18/water_filenames/middle_water_2_7_object.inc"
};
static NJS_TEXNAME water_frame_56_names[1] = {
#include "stg18/water_filenames/water_frame_56_names.inc"
};
static NJS_TEXLIST water_frame_56_texlist = {
#include "stg18/water_filenames/water_frame_56_texlist.inc"
};
static Sint16 middle_water_2_8_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_8_polygons.inc"
};
static Sint32 middle_water_2_8_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_8_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_8 = {
#include "stg18/water_filenames/middle_water_2_model_8.inc"
};
static NJS_CNK_OBJECT middle_water_2_8_object = {
#include "stg18/water_filenames/middle_water_2_8_object.inc"
};
static NJS_TEXNAME water_frame_57_names[1] = {
#include "stg18/water_filenames/water_frame_57_names.inc"
};
static NJS_TEXLIST water_frame_57_texlist = {
#include "stg18/water_filenames/water_frame_57_texlist.inc"
};
static Sint16 middle_water_2_9_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_9_polygons.inc"
};
static Sint32 middle_water_2_9_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_9_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_9 = {
#include "stg18/water_filenames/middle_water_2_model_9.inc"
};
static NJS_CNK_OBJECT middle_water_2_9_object = {
#include "stg18/water_filenames/middle_water_2_9_object.inc"
};
static NJS_TEXNAME water_frame_58_names[1] = {
#include "stg18/water_filenames/water_frame_58_names.inc"
};
static NJS_TEXLIST water_frame_58_texlist = {
#include "stg18/water_filenames/water_frame_58_texlist.inc"
};
static Sint16 middle_water_2_10_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_10_polygons.inc"
};
static Sint32 middle_water_2_10_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_10_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_10 = {
#include "stg18/water_filenames/middle_water_2_model_10.inc"
};
static NJS_CNK_OBJECT middle_water_2_10_object = {
#include "stg18/water_filenames/middle_water_2_10_object.inc"
};
static NJS_TEXNAME water_frame_59_names[1] = {
#include "stg18/water_filenames/water_frame_59_names.inc"
};
static NJS_TEXLIST water_frame_59_texlist = {
#include "stg18/water_filenames/water_frame_59_texlist.inc"
};
static Sint16 middle_water_2_11_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_11_polygons.inc"
};
static Sint32 middle_water_2_11_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_11_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_11 = {
#include "stg18/water_filenames/middle_water_2_model_11.inc"
};
static NJS_CNK_OBJECT middle_water_2_11_object = {
#include "stg18/water_filenames/middle_water_2_11_object.inc"
};
static NJS_TEXNAME water_frame_60_names[1] = {
#include "stg18/water_filenames/water_frame_60_names.inc"
};
static NJS_TEXLIST water_frame_60_texlist = {
#include "stg18/water_filenames/water_frame_60_texlist.inc"
};
static Sint16 middle_water_2_12_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_12_polygons.inc"
};
static Sint32 middle_water_2_12_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_12_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_12 = {
#include "stg18/water_filenames/middle_water_2_model_12.inc"
};
static NJS_CNK_OBJECT middle_water_2_12_object = {
#include "stg18/water_filenames/middle_water_2_12_object.inc"
};
static NJS_TEXNAME water_frame_61_names[1] = {
#include "stg18/water_filenames/water_frame_61_names.inc"
};
static NJS_TEXLIST water_frame_61_texlist = {
#include "stg18/water_filenames/water_frame_61_texlist.inc"
};
static Sint16 middle_water_2_13_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_13_polygons.inc"
};
static Sint32 middle_water_2_13_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_13_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_13 = {
#include "stg18/water_filenames/middle_water_2_model_13.inc"
};
static NJS_CNK_OBJECT middle_water_2_13_object = {
#include "stg18/water_filenames/middle_water_2_13_object.inc"
};
static NJS_TEXNAME water_frame_62_names[1] = {
#include "stg18/water_filenames/water_frame_62_names.inc"
};
static NJS_TEXLIST water_frame_62_texlist = {
#include "stg18/water_filenames/water_frame_62_texlist.inc"
};
static Sint16 middle_water_2_14_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_14_polygons.inc"
};
static Sint32 middle_water_2_14_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_14_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_14 = {
#include "stg18/water_filenames/middle_water_2_model_14.inc"
};
static NJS_CNK_OBJECT middle_water_2_14_object = {
#include "stg18/water_filenames/middle_water_2_14_object.inc"
};
static NJS_TEXNAME water_frame_63_names[1] = {
#include "stg18/water_filenames/water_frame_63_names.inc"
};
static NJS_TEXLIST water_frame_63_texlist = {
#include "stg18/water_filenames/water_frame_63_texlist.inc"
};
static Sint16 middle_water_2_15_polygons[465] = {
#include "stg18/water_filenames/middle_water_2_15_polygons.inc"
};
static Sint32 middle_water_2_15_vertices[231] = {
#include "stg18/water_filenames/middle_water_2_15_vertices.inc"
};
static NJS_CNK_MODEL middle_water_2_model_15 = {
#include "stg18/water_filenames/middle_water_2_model_15.inc"
};
static NJS_CNK_OBJECT middle_water_2_15_object = {
#include "stg18/water_filenames/middle_water_2_15_object.inc"
};
static Sint16 south_water_0_uvs[134] = {
#include "stg18/water_filenames/south_water_0_uvs.inc"
};
static struct texture_anim south_water_anim_0 = {
#include "stg18/water_filenames/south_water_anim_0.inc"
};
static Sint16 south_water_1_uvs[134] = {
#include "stg18/water_filenames/south_water_1_uvs.inc"
};
static struct texture_anim south_water_anim_1 = {
#include "stg18/water_filenames/south_water_anim_1.inc"
};
static Sint16 south_water_2_uvs[134] = {
#include "stg18/water_filenames/south_water_2_uvs.inc"
};
static struct texture_anim south_water_anim_2 = {
#include "stg18/water_filenames/south_water_anim_2.inc"
};
static Sint16 south_water_3_uvs[134] = {
#include "stg18/water_filenames/south_water_3_uvs.inc"
};
static struct texture_anim south_water_anim_3 = {
#include "stg18/water_filenames/south_water_anim_3.inc"
};
static Sint16 south_water_4_uvs[134] = {
#include "stg18/water_filenames/south_water_4_uvs.inc"
};
static struct texture_anim south_water_anim_4 = {
#include "stg18/water_filenames/south_water_anim_4.inc"
};
static Sint16 south_water_5_uvs[134] = {
#include "stg18/water_filenames/south_water_5_uvs.inc"
};
static struct texture_anim south_water_anim_5 = {
#include "stg18/water_filenames/south_water_anim_5.inc"
};
static Sint16 south_water_6_uvs[134] = {
#include "stg18/water_filenames/south_water_6_uvs.inc"
};
static struct texture_anim south_water_anim_6 = {
#include "stg18/water_filenames/south_water_anim_6.inc"
};
static Sint16 south_water_7_uvs[134] = {
#include "stg18/water_filenames/south_water_7_uvs.inc"
};
static struct texture_anim south_water_anim_7 = {
#include "stg18/water_filenames/south_water_anim_7.inc"
};
static Sint16 south_water_8_uvs[134] = {
#include "stg18/water_filenames/south_water_8_uvs.inc"
};
static struct texture_anim south_water_anim_8 = {
#include "stg18/water_filenames/south_water_anim_8.inc"
};
static Sint16 south_water_9_uvs[134] = {
#include "stg18/water_filenames/south_water_9_uvs.inc"
};
static struct texture_anim south_water_anim_9 = {
#include "stg18/water_filenames/south_water_anim_9.inc"
};
static Sint16 south_water_10_uvs[134] = {
#include "stg18/water_filenames/south_water_10_uvs.inc"
};
static struct texture_anim south_water_anim_10 = {
#include "stg18/water_filenames/south_water_anim_10.inc"
};
static Sint16 south_water_11_uvs[134] = {
#include "stg18/water_filenames/south_water_11_uvs.inc"
};
static struct texture_anim south_water_anim_11 = {
#include "stg18/water_filenames/south_water_anim_11.inc"
};
static Sint16 south_water_12_uvs[134] = {
#include "stg18/water_filenames/south_water_12_uvs.inc"
};
static struct texture_anim south_water_anim_12 = {
#include "stg18/water_filenames/south_water_anim_12.inc"
};
static Sint16 south_water_13_uvs[134] = {
#include "stg18/water_filenames/south_water_13_uvs.inc"
};
static struct texture_anim south_water_anim_13 = {
#include "stg18/water_filenames/south_water_anim_13.inc"
};
static Sint16 south_water_14_uvs[134] = {
#include "stg18/water_filenames/south_water_14_uvs.inc"
};
static struct texture_anim south_water_anim_14 = {
#include "stg18/water_filenames/south_water_anim_14.inc"
};
static Sint16 south_water_15_uvs[134] = {
#include "stg18/water_filenames/south_water_15_uvs.inc"
};
static struct texture_anim south_water_anim_15 = {
#include "stg18/water_filenames/south_water_anim_15.inc"
};
static Sint16 north_water_0_uvs[134] = {
#include "stg18/water_filenames/north_water_0_uvs.inc"
};
static struct texture_anim north_water_anim_0 = {
#include "stg18/water_filenames/north_water_anim_0.inc"
};
static Sint16 north_water_1_uvs[134] = {
#include "stg18/water_filenames/north_water_1_uvs.inc"
};
static struct texture_anim north_water_anim_1 = {
#include "stg18/water_filenames/north_water_anim_1.inc"
};
static Sint16 north_water_2_uvs[134] = {
#include "stg18/water_filenames/north_water_2_uvs.inc"
};
static struct texture_anim north_water_anim_2 = {
#include "stg18/water_filenames/north_water_anim_2.inc"
};
static Sint16 north_water_3_uvs[134] = {
#include "stg18/water_filenames/north_water_3_uvs.inc"
};
static struct texture_anim north_water_anim_3 = {
#include "stg18/water_filenames/north_water_anim_3.inc"
};
static Sint16 north_water_4_uvs[134] = {
#include "stg18/water_filenames/north_water_4_uvs.inc"
};
static struct texture_anim north_water_anim_4 = {
#include "stg18/water_filenames/north_water_anim_4.inc"
};
static Sint16 north_water_5_uvs[134] = {
#include "stg18/water_filenames/north_water_5_uvs.inc"
};
static struct texture_anim north_water_anim_5 = {
#include "stg18/water_filenames/north_water_anim_5.inc"
};
static Sint16 north_water_6_uvs[134] = {
#include "stg18/water_filenames/north_water_6_uvs.inc"
};
static struct texture_anim north_water_anim_6 = {
#include "stg18/water_filenames/north_water_anim_6.inc"
};
static Sint16 north_water_7_uvs[134] = {
#include "stg18/water_filenames/north_water_7_uvs.inc"
};
static struct texture_anim north_water_anim_7 = {
#include "stg18/water_filenames/north_water_anim_7.inc"
};
static Sint16 north_water_8_uvs[134] = {
#include "stg18/water_filenames/north_water_8_uvs.inc"
};
static struct texture_anim north_water_anim_8 = {
#include "stg18/water_filenames/north_water_anim_8.inc"
};
static Sint16 north_water_9_uvs[134] = {
#include "stg18/water_filenames/north_water_9_uvs.inc"
};
static struct texture_anim north_water_anim_9 = {
#include "stg18/water_filenames/north_water_anim_9.inc"
};
static Sint16 north_water_10_uvs[134] = {
#include "stg18/water_filenames/north_water_10_uvs.inc"
};
static struct texture_anim north_water_anim_10 = {
#include "stg18/water_filenames/north_water_anim_10.inc"
};
static Sint16 north_water_11_uvs[134] = {
#include "stg18/water_filenames/north_water_11_uvs.inc"
};
static struct texture_anim north_water_anim_11 = {
#include "stg18/water_filenames/north_water_anim_11.inc"
};
static Sint16 north_water_12_uvs[134] = {
#include "stg18/water_filenames/north_water_12_uvs.inc"
};
static struct texture_anim north_water_anim_12 = {
#include "stg18/water_filenames/north_water_anim_12.inc"
};
static Sint16 north_water_13_uvs[134] = {
#include "stg18/water_filenames/north_water_13_uvs.inc"
};
static struct texture_anim north_water_anim_13 = {
#include "stg18/water_filenames/north_water_anim_13.inc"
};
static Sint16 north_water_14_uvs[134] = {
#include "stg18/water_filenames/north_water_14_uvs.inc"
};
static struct texture_anim north_water_anim_14 = {
#include "stg18/water_filenames/north_water_anim_14.inc"
};
static Sint16 north_water_15_uvs[134] = {
#include "stg18/water_filenames/north_water_15_uvs.inc"
};
static struct texture_anim north_water_anim_15 = {
#include "stg18/water_filenames/north_water_anim_15.inc"
};
static Sint16 middle_water_0_uvs[504] = {
#include "stg18/water_filenames/middle_water_0_uvs.inc"
};
static struct texture_anim middle_water_anim_0 = {
#include "stg18/water_filenames/middle_water_anim_0.inc"
};
static Sint16 middle_water_1_uvs[504] = {
#include "stg18/water_filenames/middle_water_1_uvs.inc"
};
static struct texture_anim middle_water_anim_1 = {
#include "stg18/water_filenames/middle_water_anim_1.inc"
};
static Sint16 middle_water_2_uvs[504] = {
#include "stg18/water_filenames/middle_water_2_uvs.inc"
};
static struct texture_anim middle_water_anim_2 = {
#include "stg18/water_filenames/middle_water_anim_2.inc"
};
static Sint16 middle_water_3_uvs[504] = {
#include "stg18/water_filenames/middle_water_3_uvs.inc"
};
static struct texture_anim middle_water_anim_3 = {
#include "stg18/water_filenames/middle_water_anim_3.inc"
};
static Sint16 middle_water_4_uvs[504] = {
#include "stg18/water_filenames/middle_water_4_uvs.inc"
};
static struct texture_anim middle_water_anim_4 = {
#include "stg18/water_filenames/middle_water_anim_4.inc"
};
static Sint16 middle_water_5_uvs[504] = {
#include "stg18/water_filenames/middle_water_5_uvs.inc"
};
static struct texture_anim middle_water_anim_5 = {
#include "stg18/water_filenames/middle_water_anim_5.inc"
};
static Sint16 middle_water_6_uvs[504] = {
#include "stg18/water_filenames/middle_water_6_uvs.inc"
};
static struct texture_anim middle_water_anim_6 = {
#include "stg18/water_filenames/middle_water_anim_6.inc"
};
static Sint16 middle_water_7_uvs[504] = {
#include "stg18/water_filenames/middle_water_7_uvs.inc"
};
static struct texture_anim middle_water_anim_7 = {
#include "stg18/water_filenames/middle_water_anim_7.inc"
};
static Sint16 middle_water_8_uvs[504] = {
#include "stg18/water_filenames/middle_water_8_uvs.inc"
};
static struct texture_anim middle_water_anim_8 = {
#include "stg18/water_filenames/middle_water_anim_8.inc"
};
static Sint16 middle_water_9_uvs[504] = {
#include "stg18/water_filenames/middle_water_9_uvs.inc"
};
static struct texture_anim middle_water_anim_9 = {
#include "stg18/water_filenames/middle_water_anim_9.inc"
};
static Sint16 middle_water_10_uvs[504] = {
#include "stg18/water_filenames/middle_water_10_uvs.inc"
};
static struct texture_anim middle_water_anim_10 = {
#include "stg18/water_filenames/middle_water_anim_10.inc"
};
static Sint16 middle_water_11_uvs[504] = {
#include "stg18/water_filenames/middle_water_11_uvs.inc"
};
static struct texture_anim middle_water_anim_11 = {
#include "stg18/water_filenames/middle_water_anim_11.inc"
};
static Sint16 middle_water_12_uvs[504] = {
#include "stg18/water_filenames/middle_water_12_uvs.inc"
};
static struct texture_anim middle_water_anim_12 = {
#include "stg18/water_filenames/middle_water_anim_12.inc"
};
static Sint16 middle_water_13_uvs[504] = {
#include "stg18/water_filenames/middle_water_13_uvs.inc"
};
static struct texture_anim middle_water_anim_13 = {
#include "stg18/water_filenames/middle_water_anim_13.inc"
};
static Sint16 middle_water_14_uvs[504] = {
#include "stg18/water_filenames/middle_water_14_uvs.inc"
};
static struct texture_anim middle_water_anim_14 = {
#include "stg18/water_filenames/middle_water_anim_14.inc"
};
static Sint16 middle_water_15_uvs[504] = {
#include "stg18/water_filenames/middle_water_15_uvs.inc"
};
static struct texture_anim middle_water_anim_15 = {
#include "stg18/water_filenames/middle_water_anim_15.inc"
};
static Sint16 middle_water_2_0_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_0_uvs.inc"
};
static struct texture_anim middle_water_2_anim_0 = {
#include "stg18/water_filenames/middle_water_2_anim_0.inc"
};
static Sint16 middle_water_2_1_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_1_uvs.inc"
};
static struct texture_anim middle_water_2_anim_1 = {
#include "stg18/water_filenames/middle_water_2_anim_1.inc"
};
static Sint16 middle_water_2_2_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_2_uvs.inc"
};
static struct texture_anim middle_water_2_anim_2 = {
#include "stg18/water_filenames/middle_water_2_anim_2.inc"
};
static Sint16 middle_water_2_3_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_3_uvs.inc"
};
static struct texture_anim middle_water_2_anim_3 = {
#include "stg18/water_filenames/middle_water_2_anim_3.inc"
};
static Sint16 middle_water_2_4_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_4_uvs.inc"
};
static struct texture_anim middle_water_2_anim_4 = {
#include "stg18/water_filenames/middle_water_2_anim_4.inc"
};
static Sint16 middle_water_2_5_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_5_uvs.inc"
};
static struct texture_anim middle_water_2_anim_5 = {
#include "stg18/water_filenames/middle_water_2_anim_5.inc"
};
static Sint16 middle_water_2_6_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_6_uvs.inc"
};
static struct texture_anim middle_water_2_anim_6 = {
#include "stg18/water_filenames/middle_water_2_anim_6.inc"
};
static Sint16 middle_water_2_7_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_7_uvs.inc"
};
static struct texture_anim middle_water_2_anim_7 = {
#include "stg18/water_filenames/middle_water_2_anim_7.inc"
};
static Sint16 middle_water_2_8_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_8_uvs.inc"
};
static struct texture_anim middle_water_2_anim_8 = {
#include "stg18/water_filenames/middle_water_2_anim_8.inc"
};
static Sint16 middle_water_2_9_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_9_uvs.inc"
};
static struct texture_anim middle_water_2_anim_9 = {
#include "stg18/water_filenames/middle_water_2_anim_9.inc"
};
static Sint16 middle_water_2_10_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_10_uvs.inc"
};
static struct texture_anim middle_water_2_anim_10 = {
#include "stg18/water_filenames/middle_water_2_anim_10.inc"
};
static Sint16 middle_water_2_11_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_11_uvs.inc"
};
static struct texture_anim middle_water_2_anim_11 = {
#include "stg18/water_filenames/middle_water_2_anim_11.inc"
};
static Sint16 middle_water_2_12_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_12_uvs.inc"
};
static struct texture_anim middle_water_2_anim_12 = {
#include "stg18/water_filenames/middle_water_2_anim_12.inc"
};
static Sint16 middle_water_2_13_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_13_uvs.inc"
};
static struct texture_anim middle_water_2_anim_13 = {
#include "stg18/water_filenames/middle_water_2_anim_13.inc"
};
static Sint16 middle_water_2_14_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_14_uvs.inc"
};
static struct texture_anim middle_water_2_anim_14 = {
#include "stg18/water_filenames/middle_water_2_anim_14.inc"
};
static Sint16 middle_water_2_15_uvs[340] = {
#include "stg18/water_filenames/middle_water_2_15_uvs.inc"
};
static struct texture_anim middle_water_2_anim_15 = {
#include "stg18/water_filenames/middle_water_2_anim_15.inc"
};
static NJS_TEXNAME background_pvm_texnames[4] = {
#include "stg18/water_filenames/background_pvm_texnames.inc"
};
NJS_TEXLIST dry_lagoon_water_pvm_texlist = {
#include "stg18/water_filenames/dry_lagoon_water_pvm_texlist.inc"
};
NJS_CNK_MODEL *dry_lagoon_south_water_models[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_south_water_models.inc"
};
struct texture_anim *dry_lagoon_south_water_anims[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_south_water_anims.inc"
};
NJS_CNK_MODEL *dry_lagoon_north_water_models[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_north_water_models.inc"
};
struct texture_anim *dry_lagoon_north_water_anims[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_north_water_anims.inc"
};
NJS_CNK_MODEL *dry_lagoon_middle_water_models[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_middle_water_models.inc"
};
struct texture_anim *dry_lagoon_middle_water_anims[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_middle_water_anims.inc"
};
NJS_CNK_MODEL *dry_lagoon_middle_water_2_models[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_middle_water_2_models.inc"
};
struct texture_anim *dry_lagoon_middle_water_2_anims[LAGOON_WATER_POSES] = {
#include "stg18/water_filenames/dry_lagoon_middle_water_2_anims.inc"
};
