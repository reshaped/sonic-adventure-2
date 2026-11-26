/* STG18.PRS's texture lists, chunk models, chunk model pointers, texture
 * list pointers and texture names, from the disc. its C lies between
 * butterflies.c and stake.c, and its D lies between butterflies.c and
 * stake.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg18/plant4.h"

#include "stg18/dry_lagoon_colored_long_plant/text.inc"

static NJS_TEXNAME frame_00_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_texture_names.inc"
};
static NJS_TEXLIST frame_00_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_texture_list.inc"
};
static Sint16 frame_00_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_polygons.inc"
};
static Sint32 frame_00_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_vertices.inc"
};
static NJS_CNK_MODEL frame_00_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_model.inc"
};
static NJS_CNK_OBJECT frame_00_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_00_object.inc"
};
static NJS_TEXNAME frame_01_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_texture_names.inc"
};
static NJS_TEXLIST frame_01_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_texture_list.inc"
};
static Sint16 frame_01_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_polygons.inc"
};
static Sint32 frame_01_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_vertices.inc"
};
static NJS_CNK_MODEL frame_01_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_model.inc"
};
static NJS_CNK_OBJECT frame_01_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_01_object.inc"
};
static NJS_TEXNAME frame_02_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_texture_names.inc"
};
static NJS_TEXLIST frame_02_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_texture_list.inc"
};
static Sint16 frame_02_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_polygons.inc"
};
static Sint32 frame_02_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_vertices.inc"
};
static NJS_CNK_MODEL frame_02_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_model.inc"
};
static NJS_CNK_OBJECT frame_02_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_02_object.inc"
};
static NJS_TEXNAME frame_03_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_texture_names.inc"
};
static NJS_TEXLIST frame_03_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_texture_list.inc"
};
static Sint16 frame_03_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_polygons.inc"
};
static Sint32 frame_03_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_vertices.inc"
};
static NJS_CNK_MODEL frame_03_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_model.inc"
};
static NJS_CNK_OBJECT frame_03_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_03_object.inc"
};
static NJS_TEXNAME frame_04_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_texture_names.inc"
};
static NJS_TEXLIST frame_04_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_texture_list.inc"
};
static Sint16 frame_04_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_polygons.inc"
};
static Sint32 frame_04_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_vertices.inc"
};
static NJS_CNK_MODEL frame_04_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_model.inc"
};
static NJS_CNK_OBJECT frame_04_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_04_object.inc"
};
static NJS_TEXNAME frame_05_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_texture_names.inc"
};
static NJS_TEXLIST frame_05_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_texture_list.inc"
};
static Sint16 frame_05_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_polygons.inc"
};
static Sint32 frame_05_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_vertices.inc"
};
static NJS_CNK_MODEL frame_05_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_model.inc"
};
static NJS_CNK_OBJECT frame_05_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_05_object.inc"
};
static NJS_TEXNAME frame_06_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_texture_names.inc"
};
static NJS_TEXLIST frame_06_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_texture_list.inc"
};
static Sint16 frame_06_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_polygons.inc"
};
static Sint32 frame_06_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_vertices.inc"
};
static NJS_CNK_MODEL frame_06_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_model.inc"
};
static NJS_CNK_OBJECT frame_06_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_06_object.inc"
};
static NJS_TEXNAME frame_07_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_texture_names.inc"
};
static NJS_TEXLIST frame_07_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_texture_list.inc"
};
static Sint16 frame_07_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_polygons.inc"
};
static Sint32 frame_07_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_vertices.inc"
};
static NJS_CNK_MODEL frame_07_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_model.inc"
};
static NJS_CNK_OBJECT frame_07_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_07_object.inc"
};
static NJS_TEXNAME frame_08_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_texture_names.inc"
};
static NJS_TEXLIST frame_08_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_texture_list.inc"
};
static Sint16 frame_08_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_polygons.inc"
};
static Sint32 frame_08_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_vertices.inc"
};
static NJS_CNK_MODEL frame_08_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_model.inc"
};
static NJS_CNK_OBJECT frame_08_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_08_object.inc"
};
static NJS_TEXNAME frame_09_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_texture_names.inc"
};
static NJS_TEXLIST frame_09_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_texture_list.inc"
};
static Sint16 frame_09_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_polygons.inc"
};
static Sint32 frame_09_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_vertices.inc"
};
static NJS_CNK_MODEL frame_09_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_model.inc"
};
static NJS_CNK_OBJECT frame_09_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_09_object.inc"
};
static NJS_TEXNAME frame_10_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_texture_names.inc"
};
static NJS_TEXLIST frame_10_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_texture_list.inc"
};
static Sint16 frame_10_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_polygons.inc"
};
static Sint32 frame_10_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_vertices.inc"
};
static NJS_CNK_MODEL frame_10_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_model.inc"
};
static NJS_CNK_OBJECT frame_10_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_10_object.inc"
};
static NJS_TEXNAME frame_11_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_texture_names.inc"
};
static NJS_TEXLIST frame_11_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_texture_list.inc"
};
static Sint16 frame_11_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_polygons.inc"
};
static Sint32 frame_11_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_vertices.inc"
};
static NJS_CNK_MODEL frame_11_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_model.inc"
};
static NJS_CNK_OBJECT frame_11_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_11_object.inc"
};
static NJS_TEXNAME frame_12_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_texture_names.inc"
};
static NJS_TEXLIST frame_12_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_texture_list.inc"
};
static Sint16 frame_12_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_polygons.inc"
};
static Sint32 frame_12_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_vertices.inc"
};
static NJS_CNK_MODEL frame_12_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_model.inc"
};
static NJS_CNK_OBJECT frame_12_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_12_object.inc"
};
static NJS_TEXNAME frame_13_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_texture_names.inc"
};
static NJS_TEXLIST frame_13_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_texture_list.inc"
};
static Sint16 frame_13_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_polygons.inc"
};
static Sint32 frame_13_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_vertices.inc"
};
static NJS_CNK_MODEL frame_13_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_model.inc"
};
static NJS_CNK_OBJECT frame_13_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_13_object.inc"
};
static NJS_TEXNAME frame_14_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_texture_names.inc"
};
static NJS_TEXLIST frame_14_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_texture_list.inc"
};
static Sint16 frame_14_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_polygons.inc"
};
static Sint32 frame_14_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_vertices.inc"
};
static NJS_CNK_MODEL frame_14_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_model.inc"
};
static NJS_CNK_OBJECT frame_14_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_14_object.inc"
};
static NJS_TEXNAME frame_15_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_texture_names.inc"
};
static NJS_TEXLIST frame_15_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_texture_list.inc"
};
static Sint16 frame_15_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_polygons.inc"
};
static Sint32 frame_15_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_vertices.inc"
};
static NJS_CNK_MODEL frame_15_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_model.inc"
};
static NJS_CNK_OBJECT frame_15_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_15_object.inc"
};
static NJS_TEXNAME frame_16_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_texture_names.inc"
};
static NJS_TEXLIST frame_16_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_texture_list.inc"
};
static Sint16 frame_16_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_polygons.inc"
};
static Sint32 frame_16_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_vertices.inc"
};
static NJS_CNK_MODEL frame_16_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_model.inc"
};
static NJS_CNK_OBJECT frame_16_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_16_object.inc"
};
static NJS_TEXNAME frame_17_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_texture_names.inc"
};
static NJS_TEXLIST frame_17_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_texture_list.inc"
};
static Sint16 frame_17_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_polygons.inc"
};
static Sint32 frame_17_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_vertices.inc"
};
static NJS_CNK_MODEL frame_17_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_model.inc"
};
static NJS_CNK_OBJECT frame_17_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_17_object.inc"
};
static NJS_TEXNAME frame_18_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_texture_names.inc"
};
static NJS_TEXLIST frame_18_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_texture_list.inc"
};
static Sint16 frame_18_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_polygons.inc"
};
static Sint32 frame_18_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_vertices.inc"
};
static NJS_CNK_MODEL frame_18_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_model.inc"
};
static NJS_CNK_OBJECT frame_18_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_18_object.inc"
};
static NJS_TEXNAME frame_19_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_texture_names.inc"
};
static NJS_TEXLIST frame_19_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_texture_list.inc"
};
static Sint16 frame_19_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_polygons.inc"
};
static Sint32 frame_19_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_vertices.inc"
};
static NJS_CNK_MODEL frame_19_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_model.inc"
};
static NJS_CNK_OBJECT frame_19_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_19_object.inc"
};
static NJS_TEXNAME frame_20_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_texture_names.inc"
};
static NJS_TEXLIST frame_20_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_texture_list.inc"
};
static Sint16 frame_20_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_polygons.inc"
};
static Sint32 frame_20_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_vertices.inc"
};
static NJS_CNK_MODEL frame_20_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_model.inc"
};
static NJS_CNK_OBJECT frame_20_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_20_object.inc"
};
static NJS_TEXNAME frame_21_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_texture_names.inc"
};
static NJS_TEXLIST frame_21_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_texture_list.inc"
};
static Sint16 frame_21_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_polygons.inc"
};
static Sint32 frame_21_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_vertices.inc"
};
static NJS_CNK_MODEL frame_21_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_model.inc"
};
static NJS_CNK_OBJECT frame_21_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_21_object.inc"
};
static NJS_TEXNAME frame_22_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_texture_names.inc"
};
static NJS_TEXLIST frame_22_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_texture_list.inc"
};
static Sint16 frame_22_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_polygons.inc"
};
static Sint32 frame_22_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_vertices.inc"
};
static NJS_CNK_MODEL frame_22_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_model.inc"
};
static NJS_CNK_OBJECT frame_22_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_22_object.inc"
};
static NJS_TEXNAME frame_23_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_texture_names.inc"
};
static NJS_TEXLIST frame_23_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_texture_list.inc"
};
static Sint16 frame_23_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_polygons.inc"
};
static Sint32 frame_23_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_vertices.inc"
};
static NJS_CNK_MODEL frame_23_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_model.inc"
};
static NJS_CNK_OBJECT frame_23_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_23_object.inc"
};
static NJS_TEXNAME frame_24_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_texture_names.inc"
};
static NJS_TEXLIST frame_24_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_texture_list.inc"
};
static Sint16 frame_24_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_polygons.inc"
};
static Sint32 frame_24_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_vertices.inc"
};
static NJS_CNK_MODEL frame_24_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_model.inc"
};
static NJS_CNK_OBJECT frame_24_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_24_object.inc"
};
static NJS_TEXNAME frame_25_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_texture_names.inc"
};
static NJS_TEXLIST frame_25_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_texture_list.inc"
};
static Sint16 frame_25_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_polygons.inc"
};
static Sint32 frame_25_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_vertices.inc"
};
static NJS_CNK_MODEL frame_25_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_model.inc"
};
static NJS_CNK_OBJECT frame_25_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_25_object.inc"
};
static NJS_TEXNAME frame_26_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_texture_names.inc"
};
static NJS_TEXLIST frame_26_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_texture_list.inc"
};
static Sint16 frame_26_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_polygons.inc"
};
static Sint32 frame_26_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_vertices.inc"
};
static NJS_CNK_MODEL frame_26_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_model.inc"
};
static NJS_CNK_OBJECT frame_26_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_26_object.inc"
};
static NJS_TEXNAME frame_27_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_texture_names.inc"
};
static NJS_TEXLIST frame_27_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_texture_list.inc"
};
static Sint16 frame_27_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_polygons.inc"
};
static Sint32 frame_27_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_vertices.inc"
};
static NJS_CNK_MODEL frame_27_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_model.inc"
};
static NJS_CNK_OBJECT frame_27_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_27_object.inc"
};
static NJS_TEXNAME frame_28_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_texture_names.inc"
};
static NJS_TEXLIST frame_28_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_texture_list.inc"
};
static Sint16 frame_28_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_polygons.inc"
};
static Sint32 frame_28_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_vertices.inc"
};
static NJS_CNK_MODEL frame_28_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_model.inc"
};
static NJS_CNK_OBJECT frame_28_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_28_object.inc"
};
static NJS_TEXNAME frame_29_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_texture_names.inc"
};
static NJS_TEXLIST frame_29_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_texture_list.inc"
};
static Sint16 frame_29_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_polygons.inc"
};
static Sint32 frame_29_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_vertices.inc"
};
static NJS_CNK_MODEL frame_29_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_model.inc"
};
static NJS_CNK_OBJECT frame_29_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_29_object.inc"
};
static NJS_TEXNAME frame_30_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_texture_names.inc"
};
static NJS_TEXLIST frame_30_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_texture_list.inc"
};
static Sint16 frame_30_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_polygons.inc"
};
static Sint32 frame_30_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_vertices.inc"
};
static NJS_CNK_MODEL frame_30_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_model.inc"
};
static NJS_CNK_OBJECT frame_30_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_30_object.inc"
};
static NJS_TEXNAME frame_31_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_texture_names.inc"
};
static NJS_TEXLIST frame_31_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_texture_list.inc"
};
static Sint16 frame_31_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_polygons.inc"
};
static Sint32 frame_31_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_vertices.inc"
};
static NJS_CNK_MODEL frame_31_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_model.inc"
};
static NJS_CNK_OBJECT frame_31_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_31_object.inc"
};
static NJS_TEXNAME frame_32_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_texture_names.inc"
};
static NJS_TEXLIST frame_32_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_texture_list.inc"
};
static Sint16 frame_32_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_polygons.inc"
};
static Sint32 frame_32_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_vertices.inc"
};
static NJS_CNK_MODEL frame_32_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_model.inc"
};
static NJS_CNK_OBJECT frame_32_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_32_object.inc"
};
static NJS_TEXNAME frame_33_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_texture_names.inc"
};
static NJS_TEXLIST frame_33_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_texture_list.inc"
};
static Sint16 frame_33_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_polygons.inc"
};
static Sint32 frame_33_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_vertices.inc"
};
static NJS_CNK_MODEL frame_33_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_model.inc"
};
static NJS_CNK_OBJECT frame_33_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_33_object.inc"
};
static NJS_TEXNAME frame_34_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_texture_names.inc"
};
static NJS_TEXLIST frame_34_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_texture_list.inc"
};
static Sint16 frame_34_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_polygons.inc"
};
static Sint32 frame_34_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_vertices.inc"
};
static NJS_CNK_MODEL frame_34_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_model.inc"
};
static NJS_CNK_OBJECT frame_34_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_34_object.inc"
};
static NJS_TEXNAME frame_35_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_texture_names.inc"
};
static NJS_TEXLIST frame_35_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_texture_list.inc"
};
static Sint16 frame_35_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_polygons.inc"
};
static Sint32 frame_35_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_vertices.inc"
};
static NJS_CNK_MODEL frame_35_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_model.inc"
};
static NJS_CNK_OBJECT frame_35_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_35_object.inc"
};
static NJS_TEXNAME frame_36_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_texture_names.inc"
};
static NJS_TEXLIST frame_36_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_texture_list.inc"
};
static Sint16 frame_36_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_polygons.inc"
};
static Sint32 frame_36_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_vertices.inc"
};
static NJS_CNK_MODEL frame_36_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_model.inc"
};
static NJS_CNK_OBJECT frame_36_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_36_object.inc"
};
static NJS_TEXNAME frame_37_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_texture_names.inc"
};
static NJS_TEXLIST frame_37_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_texture_list.inc"
};
static Sint16 frame_37_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_polygons.inc"
};
static Sint32 frame_37_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_vertices.inc"
};
static NJS_CNK_MODEL frame_37_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_model.inc"
};
static NJS_CNK_OBJECT frame_37_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_37_object.inc"
};
static NJS_TEXNAME frame_38_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_texture_names.inc"
};
static NJS_TEXLIST frame_38_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_texture_list.inc"
};
static Sint16 frame_38_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_polygons.inc"
};
static Sint32 frame_38_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_vertices.inc"
};
static NJS_CNK_MODEL frame_38_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_model.inc"
};
static NJS_CNK_OBJECT frame_38_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_38_object.inc"
};
static NJS_TEXNAME frame_39_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_texture_names.inc"
};
static NJS_TEXLIST frame_39_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_texture_list.inc"
};
static Sint16 frame_39_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_polygons.inc"
};
static Sint32 frame_39_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_vertices.inc"
};
static NJS_CNK_MODEL frame_39_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_model.inc"
};
static NJS_CNK_OBJECT frame_39_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_39_object.inc"
};
static NJS_TEXNAME frame_40_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_texture_names.inc"
};
static NJS_TEXLIST frame_40_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_texture_list.inc"
};
static Sint16 frame_40_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_polygons.inc"
};
static Sint32 frame_40_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_vertices.inc"
};
static NJS_CNK_MODEL frame_40_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_model.inc"
};
static NJS_CNK_OBJECT frame_40_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_40_object.inc"
};
static NJS_TEXNAME frame_41_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_texture_names.inc"
};
static NJS_TEXLIST frame_41_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_texture_list.inc"
};
static Sint16 frame_41_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_polygons.inc"
};
static Sint32 frame_41_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_vertices.inc"
};
static NJS_CNK_MODEL frame_41_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_model.inc"
};
static NJS_CNK_OBJECT frame_41_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_41_object.inc"
};
static NJS_TEXNAME frame_42_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_texture_names.inc"
};
static NJS_TEXLIST frame_42_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_texture_list.inc"
};
static Sint16 frame_42_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_polygons.inc"
};
static Sint32 frame_42_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_vertices.inc"
};
static NJS_CNK_MODEL frame_42_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_model.inc"
};
static NJS_CNK_OBJECT frame_42_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_42_object.inc"
};
static NJS_TEXNAME frame_43_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_texture_names.inc"
};
static NJS_TEXLIST frame_43_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_texture_list.inc"
};
static Sint16 frame_43_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_polygons.inc"
};
static Sint32 frame_43_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_vertices.inc"
};
static NJS_CNK_MODEL frame_43_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_model.inc"
};
static NJS_CNK_OBJECT frame_43_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_43_object.inc"
};
static NJS_TEXNAME frame_44_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_texture_names.inc"
};
static NJS_TEXLIST frame_44_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_texture_list.inc"
};
static Sint16 frame_44_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_polygons.inc"
};
static Sint32 frame_44_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_vertices.inc"
};
static NJS_CNK_MODEL frame_44_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_model.inc"
};
static NJS_CNK_OBJECT frame_44_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_44_object.inc"
};
static NJS_TEXNAME frame_45_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_texture_names.inc"
};
static NJS_TEXLIST frame_45_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_texture_list.inc"
};
static Sint16 frame_45_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_polygons.inc"
};
static Sint32 frame_45_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_vertices.inc"
};
static NJS_CNK_MODEL frame_45_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_model.inc"
};
static NJS_CNK_OBJECT frame_45_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_45_object.inc"
};
static NJS_TEXNAME frame_46_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_texture_names.inc"
};
static NJS_TEXLIST frame_46_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_texture_list.inc"
};
static Sint16 frame_46_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_polygons.inc"
};
static Sint32 frame_46_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_vertices.inc"
};
static NJS_CNK_MODEL frame_46_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_model.inc"
};
static NJS_CNK_OBJECT frame_46_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_46_object.inc"
};
static NJS_TEXNAME frame_47_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_texture_names.inc"
};
static NJS_TEXLIST frame_47_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_texture_list.inc"
};
static Sint16 frame_47_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_polygons.inc"
};
static Sint32 frame_47_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_vertices.inc"
};
static NJS_CNK_MODEL frame_47_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_model.inc"
};
static NJS_CNK_OBJECT frame_47_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_47_object.inc"
};
static NJS_TEXNAME frame_48_texture_names[1] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_texture_names.inc"
};
static NJS_TEXLIST frame_48_texture_list = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_texture_list.inc"
};
static Sint16 frame_48_polygons[109] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_polygons.inc"
};
static Sint32 frame_48_vertices[99] = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_vertices.inc"
};
static NJS_CNK_MODEL frame_48_model = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_model.inc"
};
static NJS_CNK_OBJECT frame_48_object = {
#include "stg18/dry_lagoon_colored_long_plant/frame_48_object.inc"
};
NJS_CNK_MODEL *plant4_models[49] = {
#include "stg18/dry_lagoon_colored_long_plant/plant4_models.inc"
};
NJS_TEXLIST *plant4_texlists[50] = {
#include "stg18/dry_lagoon_colored_long_plant/plant4_texlists.inc"
};
