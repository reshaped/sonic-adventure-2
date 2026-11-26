#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/core_scenery_art.h"
#include "stg34/scenery.h"
#include "stg34/scenery_art.h"

/* the file names that the scenery's texture lists point at. each starts on a
 * 4-byte boundary */
const char cannons_core_scenery_filenames[258] = {
#include "stg34/scenery_art/cannons_core_scenery_filenames.inc"
};

/* STG34.PRS's chunk models, texture lists, motions, UV scrolls, Basic
 * models, texture list pointers, words and chunk model pointers, from the
 * disc. its D lies between objects.c and ambient_levels.c */
static Sint16 inner_wall_entry_part_1_polygons[163] = {
#include "stg34/scenery_art/inner_wall_entry_part_1_polygons.inc"
};
static Sint32 inner_wall_entry_part_1_vertices[291] = {
#include "stg34/scenery_art/inner_wall_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_entry_part_1_model = {
#include "stg34/scenery_art/inner_wall_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT inner_wall_entry_part_1 = {
#include "stg34/scenery_art/inner_wall_entry_part_1.inc"
};
NJS_CNK_OBJECT inner_wall_entry = {
#include "stg34/scenery_art/inner_wall_entry.inc"
};
static Sint16 inner_wall_shaft_part_1_polygons[163] = {
#include "stg34/scenery_art/inner_wall_shaft_part_1_polygons.inc"
};
static Sint32 inner_wall_shaft_part_1_vertices[291] = {
#include "stg34/scenery_art/inner_wall_shaft_part_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_shaft_part_1_model = {
#include "stg34/scenery_art/inner_wall_shaft_part_1_model.inc"
};
static NJS_CNK_OBJECT inner_wall_shaft_part_1 = {
#include "stg34/scenery_art/inner_wall_shaft_part_1.inc"
};
NJS_CNK_OBJECT inner_wall_shaft = {
#include "stg34/scenery_art/inner_wall_shaft.inc"
};
static Sint16 middle_wall_entry_part_5_polygons[291] = {
#include "stg34/scenery_art/middle_wall_entry_part_5_polygons.inc"
};
static Sint32 middle_wall_entry_part_5_vertices[547] = {
#include "stg34/scenery_art/middle_wall_entry_part_5_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_part_5_model = {
#include "stg34/scenery_art/middle_wall_entry_part_5_model.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_part_5 = {
#include "stg34/scenery_art/middle_wall_entry_part_5.inc"
};
static Sint16 middle_wall_entry_part_4_polygons[291] = {
#include "stg34/scenery_art/middle_wall_entry_part_4_polygons.inc"
};
static Sint32 middle_wall_entry_part_4_vertices[547] = {
#include "stg34/scenery_art/middle_wall_entry_part_4_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_part_4_model = {
#include "stg34/scenery_art/middle_wall_entry_part_4_model.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_part_4 = {
#include "stg34/scenery_art/middle_wall_entry_part_4.inc"
};
static Sint16 middle_wall_entry_part_3_polygons[291] = {
#include "stg34/scenery_art/middle_wall_entry_part_3_polygons.inc"
};
static Sint32 middle_wall_entry_part_3_vertices[547] = {
#include "stg34/scenery_art/middle_wall_entry_part_3_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_part_3_model = {
#include "stg34/scenery_art/middle_wall_entry_part_3_model.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_part_3 = {
#include "stg34/scenery_art/middle_wall_entry_part_3.inc"
};
static Sint16 middle_wall_entry_part_2_polygons[291] = {
#include "stg34/scenery_art/middle_wall_entry_part_2_polygons.inc"
};
static Sint32 middle_wall_entry_part_2_vertices[547] = {
#include "stg34/scenery_art/middle_wall_entry_part_2_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_part_2_model = {
#include "stg34/scenery_art/middle_wall_entry_part_2_model.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_part_2 = {
#include "stg34/scenery_art/middle_wall_entry_part_2.inc"
};
static Sint16 middle_wall_entry_part_1_polygons[291] = {
#include "stg34/scenery_art/middle_wall_entry_part_1_polygons.inc"
};
static Sint32 middle_wall_entry_part_1_vertices[547] = {
#include "stg34/scenery_art/middle_wall_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_entry_part_1_model = {
#include "stg34/scenery_art/middle_wall_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT middle_wall_entry_part_1 = {
#include "stg34/scenery_art/middle_wall_entry_part_1.inc"
};
NJS_CNK_OBJECT middle_wall_entry = {
#include "stg34/scenery_art/middle_wall_entry.inc"
};
static Sint16 middle_wall_rotor_part_1_polygons[291] = {
#include "stg34/scenery_art/middle_wall_rotor_part_1_polygons.inc"
};
static Sint32 middle_wall_rotor_part_1_vertices[547] = {
#include "stg34/scenery_art/middle_wall_rotor_part_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_rotor_part_1_model = {
#include "stg34/scenery_art/middle_wall_rotor_part_1_model.inc"
};
static NJS_CNK_OBJECT middle_wall_rotor_part_1 = {
#include "stg34/scenery_art/middle_wall_rotor_part_1.inc"
};
NJS_CNK_OBJECT middle_wall_rotor = {
#include "stg34/scenery_art/middle_wall_rotor.inc"
};
static Sint16 middle_wall_drop_part_4_polygons[291] = {
#include "stg34/scenery_art/middle_wall_drop_part_4_polygons.inc"
};
static Sint32 middle_wall_drop_part_4_vertices[547] = {
#include "stg34/scenery_art/middle_wall_drop_part_4_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_drop_part_4_model = {
#include "stg34/scenery_art/middle_wall_drop_part_4_model.inc"
};
static NJS_CNK_OBJECT middle_wall_drop_part_4 = {
#include "stg34/scenery_art/middle_wall_drop_part_4.inc"
};
static Sint16 middle_wall_drop_part_3_polygons[291] = {
#include "stg34/scenery_art/middle_wall_drop_part_3_polygons.inc"
};
static Sint32 middle_wall_drop_part_3_vertices[547] = {
#include "stg34/scenery_art/middle_wall_drop_part_3_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_drop_part_3_model = {
#include "stg34/scenery_art/middle_wall_drop_part_3_model.inc"
};
static NJS_CNK_OBJECT middle_wall_drop_part_3 = {
#include "stg34/scenery_art/middle_wall_drop_part_3.inc"
};
static Sint16 middle_wall_drop_part_2_polygons[291] = {
#include "stg34/scenery_art/middle_wall_drop_part_2_polygons.inc"
};
static Sint32 middle_wall_drop_part_2_vertices[547] = {
#include "stg34/scenery_art/middle_wall_drop_part_2_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_drop_part_2_model = {
#include "stg34/scenery_art/middle_wall_drop_part_2_model.inc"
};
static NJS_CNK_OBJECT middle_wall_drop_part_2 = {
#include "stg34/scenery_art/middle_wall_drop_part_2.inc"
};
static Sint16 middle_wall_drop_part_1_polygons[291] = {
#include "stg34/scenery_art/middle_wall_drop_part_1_polygons.inc"
};
static Sint32 middle_wall_drop_part_1_vertices[547] = {
#include "stg34/scenery_art/middle_wall_drop_part_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_drop_part_1_model = {
#include "stg34/scenery_art/middle_wall_drop_part_1_model.inc"
};
static NJS_CNK_OBJECT middle_wall_drop_part_1 = {
#include "stg34/scenery_art/middle_wall_drop_part_1.inc"
};
NJS_CNK_OBJECT middle_wall_drop = {
#include "stg34/scenery_art/middle_wall_drop.inc"
};
static Sint16 outer_wall_entry_part_1_polygons[147] = {
#include "stg34/scenery_art/outer_wall_entry_part_1_polygons.inc"
};
static Sint32 outer_wall_entry_part_1_vertices[259] = {
#include "stg34/scenery_art/outer_wall_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_entry_part_1_model = {
#include "stg34/scenery_art/outer_wall_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT outer_wall_entry_part_1 = {
#include "stg34/scenery_art/outer_wall_entry_part_1.inc"
};
NJS_CNK_OBJECT outer_wall_entry = {
#include "stg34/scenery_art/outer_wall_entry.inc"
};
static Sint16 outer_wall_drop_part_1_polygons[147] = {
#include "stg34/scenery_art/outer_wall_drop_part_1_polygons.inc"
};
static Sint32 outer_wall_drop_part_1_vertices[259] = {
#include "stg34/scenery_art/outer_wall_drop_part_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_drop_part_1_model = {
#include "stg34/scenery_art/outer_wall_drop_part_1_model.inc"
};
static NJS_CNK_OBJECT outer_wall_drop_part_1 = {
#include "stg34/scenery_art/outer_wall_drop_part_1.inc"
};
NJS_CNK_OBJECT outer_wall_drop = {
#include "stg34/scenery_art/outer_wall_drop.inc"
};
static NJS_TEXNAME inner_wall_textures_names[1] = {
#include "stg34/scenery_art/inner_wall_textures_names.inc"
};
NJS_TEXLIST inner_wall_textures = {
#include "stg34/scenery_art/inner_wall_textures.inc"
};
static Sint16 inner_wall_frame_7_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_7_polygons.inc"
};
static Sint32 inner_wall_frame_7_vertices[307] = {
#include "stg34/scenery_art/inner_wall_frame_7_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_7_model = {
#include "stg34/scenery_art/inner_wall_frame_7_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_7_object = {
#include "stg34/scenery_art/inner_wall_frame_7_object.inc"
};
static Sint16 inner_wall_frame_6_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_6_polygons.inc"
};
static Sint32 inner_wall_frame_6_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_6_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_6_model = {
#include "stg34/scenery_art/inner_wall_frame_6_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_6_object = {
#include "stg34/scenery_art/inner_wall_frame_6_object.inc"
};
static Sint16 inner_wall_frame_5_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_5_polygons.inc"
};
static Sint32 inner_wall_frame_5_vertices[295] = {
#include "stg34/scenery_art/inner_wall_frame_5_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_5_model = {
#include "stg34/scenery_art/inner_wall_frame_5_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_5_object = {
#include "stg34/scenery_art/inner_wall_frame_5_object.inc"
};
static Sint16 inner_wall_frame_4_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_4_polygons.inc"
};
static Sint32 inner_wall_frame_4_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_4_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_4_model = {
#include "stg34/scenery_art/inner_wall_frame_4_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_4_object = {
#include "stg34/scenery_art/inner_wall_frame_4_object.inc"
};
static Sint16 inner_wall_frame_3_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_3_polygons.inc"
};
static Sint32 inner_wall_frame_3_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_3_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_3_model = {
#include "stg34/scenery_art/inner_wall_frame_3_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_3_object = {
#include "stg34/scenery_art/inner_wall_frame_3_object.inc"
};
static Sint16 inner_wall_frame_2_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_2_polygons.inc"
};
static Sint32 inner_wall_frame_2_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_2_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_2_model = {
#include "stg34/scenery_art/inner_wall_frame_2_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_2_object = {
#include "stg34/scenery_art/inner_wall_frame_2_object.inc"
};
static Sint16 inner_wall_frame_1_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_1_polygons.inc"
};
static Sint32 inner_wall_frame_1_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_1_model = {
#include "stg34/scenery_art/inner_wall_frame_1_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_1_object = {
#include "stg34/scenery_art/inner_wall_frame_1_object.inc"
};
static Sint16 inner_wall_frame_0_polygons[467] = {
#include "stg34/scenery_art/inner_wall_frame_0_polygons.inc"
};
static Sint32 inner_wall_frame_0_vertices[299] = {
#include "stg34/scenery_art/inner_wall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL inner_wall_frame_0_model = {
#include "stg34/scenery_art/inner_wall_frame_0_model.inc"
};
static NJS_CNK_OBJECT inner_wall_frame_0_object = {
#include "stg34/scenery_art/inner_wall_frame_0_object.inc"
};
static NJS_CNK_OBJECT inner_wall_sequence_part_0 = {
#include "stg34/scenery_art/inner_wall_sequence_part_0.inc"
};
static NJS_TEXNAME middle_wall_textures_names[1] = {
#include "stg34/scenery_art/middle_wall_textures_names.inc"
};
NJS_TEXLIST middle_wall_textures = {
#include "stg34/scenery_art/middle_wall_textures.inc"
};
static Sint16 middle_wall_frame_15_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_15_polygons.inc"
};
static Sint32 middle_wall_frame_15_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_15_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_15_model = {
#include "stg34/scenery_art/middle_wall_frame_15_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_15_object = {
#include "stg34/scenery_art/middle_wall_frame_15_object.inc"
};
static Sint16 middle_wall_frame_14_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_14_polygons.inc"
};
static Sint32 middle_wall_frame_14_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_14_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_14_model = {
#include "stg34/scenery_art/middle_wall_frame_14_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_14_object = {
#include "stg34/scenery_art/middle_wall_frame_14_object.inc"
};
static Sint16 middle_wall_frame_13_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_13_polygons.inc"
};
static Sint32 middle_wall_frame_13_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_13_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_13_model = {
#include "stg34/scenery_art/middle_wall_frame_13_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_13_object = {
#include "stg34/scenery_art/middle_wall_frame_13_object.inc"
};
static Sint16 middle_wall_frame_12_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_12_polygons.inc"
};
static Sint32 middle_wall_frame_12_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_12_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_12_model = {
#include "stg34/scenery_art/middle_wall_frame_12_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_12_object = {
#include "stg34/scenery_art/middle_wall_frame_12_object.inc"
};
static Sint16 middle_wall_frame_11_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_11_polygons.inc"
};
static Sint32 middle_wall_frame_11_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_11_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_11_model = {
#include "stg34/scenery_art/middle_wall_frame_11_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_11_object = {
#include "stg34/scenery_art/middle_wall_frame_11_object.inc"
};
static Sint16 middle_wall_frame_10_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_10_polygons.inc"
};
static Sint32 middle_wall_frame_10_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_10_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_10_model = {
#include "stg34/scenery_art/middle_wall_frame_10_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_10_object = {
#include "stg34/scenery_art/middle_wall_frame_10_object.inc"
};
static Sint16 middle_wall_frame_9_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_9_polygons.inc"
};
static Sint32 middle_wall_frame_9_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_9_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_9_model = {
#include "stg34/scenery_art/middle_wall_frame_9_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_9_object = {
#include "stg34/scenery_art/middle_wall_frame_9_object.inc"
};
static Sint16 middle_wall_frame_8_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_8_polygons.inc"
};
static Sint32 middle_wall_frame_8_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_8_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_8_model = {
#include "stg34/scenery_art/middle_wall_frame_8_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_8_object = {
#include "stg34/scenery_art/middle_wall_frame_8_object.inc"
};
static Sint16 middle_wall_frame_7_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_7_polygons.inc"
};
static Sint32 middle_wall_frame_7_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_7_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_7_model = {
#include "stg34/scenery_art/middle_wall_frame_7_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_7_object = {
#include "stg34/scenery_art/middle_wall_frame_7_object.inc"
};
static Sint16 middle_wall_frame_6_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_6_polygons.inc"
};
static Sint32 middle_wall_frame_6_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_6_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_6_model = {
#include "stg34/scenery_art/middle_wall_frame_6_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_6_object = {
#include "stg34/scenery_art/middle_wall_frame_6_object.inc"
};
static Sint16 middle_wall_frame_5_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_5_polygons.inc"
};
static Sint32 middle_wall_frame_5_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_5_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_5_model = {
#include "stg34/scenery_art/middle_wall_frame_5_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_5_object = {
#include "stg34/scenery_art/middle_wall_frame_5_object.inc"
};
static Sint16 middle_wall_frame_4_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_4_polygons.inc"
};
static Sint32 middle_wall_frame_4_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_4_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_4_model = {
#include "stg34/scenery_art/middle_wall_frame_4_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_4_object = {
#include "stg34/scenery_art/middle_wall_frame_4_object.inc"
};
static Sint16 middle_wall_frame_3_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_3_polygons.inc"
};
static Sint32 middle_wall_frame_3_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_3_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_3_model = {
#include "stg34/scenery_art/middle_wall_frame_3_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_3_object = {
#include "stg34/scenery_art/middle_wall_frame_3_object.inc"
};
static Sint16 middle_wall_frame_2_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_2_polygons.inc"
};
static Sint32 middle_wall_frame_2_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_2_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_2_model = {
#include "stg34/scenery_art/middle_wall_frame_2_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_2_object = {
#include "stg34/scenery_art/middle_wall_frame_2_object.inc"
};
static Sint16 middle_wall_frame_1_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_1_polygons.inc"
};
static Sint32 middle_wall_frame_1_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_1_model = {
#include "stg34/scenery_art/middle_wall_frame_1_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_1_object = {
#include "stg34/scenery_art/middle_wall_frame_1_object.inc"
};
static Sint16 middle_wall_frame_0_polygons[837] = {
#include "stg34/scenery_art/middle_wall_frame_0_polygons.inc"
};
static Sint32 middle_wall_frame_0_vertices[547] = {
#include "stg34/scenery_art/middle_wall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL middle_wall_frame_0_model = {
#include "stg34/scenery_art/middle_wall_frame_0_model.inc"
};
static NJS_CNK_OBJECT middle_wall_frame_0_object = {
#include "stg34/scenery_art/middle_wall_frame_0_object.inc"
};
static NJS_CNK_OBJECT middle_wall_sequence_part_0 = {
#include "stg34/scenery_art/middle_wall_sequence_part_0.inc"
};
static NJS_TEXNAME outer_wall_textures_names[1] = {
#include "stg34/scenery_art/outer_wall_textures_names.inc"
};
NJS_TEXLIST outer_wall_textures = {
#include "stg34/scenery_art/outer_wall_textures.inc"
};
static Sint16 outer_wall_frame_1_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_1_polygons.inc"
};
static Sint32 outer_wall_frame_1_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_1_model = {
#include "stg34/scenery_art/outer_wall_frame_1_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_1_object = {
#include "stg34/scenery_art/outer_wall_frame_1_object.inc"
};
static Sint16 outer_wall_frame_10_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_10_polygons.inc"
};
static Sint32 outer_wall_frame_10_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_10_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_10_model = {
#include "stg34/scenery_art/outer_wall_frame_10_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_10_object = {
#include "stg34/scenery_art/outer_wall_frame_10_object.inc"
};
static Sint16 outer_wall_frame_11_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_11_polygons.inc"
};
static Sint32 outer_wall_frame_11_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_11_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_11_model = {
#include "stg34/scenery_art/outer_wall_frame_11_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_11_object = {
#include "stg34/scenery_art/outer_wall_frame_11_object.inc"
};
static Sint16 outer_wall_frame_12_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_12_polygons.inc"
};
static Sint32 outer_wall_frame_12_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_12_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_12_model = {
#include "stg34/scenery_art/outer_wall_frame_12_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_12_object = {
#include "stg34/scenery_art/outer_wall_frame_12_object.inc"
};
static Sint16 outer_wall_frame_13_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_13_polygons.inc"
};
static Sint32 outer_wall_frame_13_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_13_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_13_model = {
#include "stg34/scenery_art/outer_wall_frame_13_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_13_object = {
#include "stg34/scenery_art/outer_wall_frame_13_object.inc"
};
static Sint16 outer_wall_frame_14_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_14_polygons.inc"
};
static Sint32 outer_wall_frame_14_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_14_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_14_model = {
#include "stg34/scenery_art/outer_wall_frame_14_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_14_object = {
#include "stg34/scenery_art/outer_wall_frame_14_object.inc"
};
static Sint16 outer_wall_frame_15_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_15_polygons.inc"
};
static Sint32 outer_wall_frame_15_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_15_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_15_model = {
#include "stg34/scenery_art/outer_wall_frame_15_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_15_object = {
#include "stg34/scenery_art/outer_wall_frame_15_object.inc"
};
static Sint16 outer_wall_frame_0_polygons[405] = {
#include "stg34/scenery_art/outer_wall_frame_0_polygons.inc"
};
static Sint32 outer_wall_frame_0_vertices[259] = {
#include "stg34/scenery_art/outer_wall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL outer_wall_frame_0_model = {
#include "stg34/scenery_art/outer_wall_frame_0_model.inc"
};
static NJS_CNK_OBJECT outer_wall_frame_0_object = {
#include "stg34/scenery_art/outer_wall_frame_0_object.inc"
};
static NJS_CNK_OBJECT outer_wall_sequence_part_0 = {
#include "stg34/scenery_art/outer_wall_sequence_part_0.inc"
};
static Sint16 entrance_units_part_2_polygons[49] = {
#include "stg34/scenery_art/entrance_units_part_2_polygons.inc"
};
static Sint32 entrance_units_part_2_vertices[99] = {
#include "stg34/scenery_art/entrance_units_part_2_vertices.inc"
};
static NJS_CNK_MODEL entrance_units_part_2_model = {
#include "stg34/scenery_art/entrance_units_part_2_model.inc"
};
static NJS_CNK_OBJECT entrance_units_part_2 = {
#include "stg34/scenery_art/entrance_units_part_2.inc"
};
static Sint16 entrance_units_part_1_polygons[49] = {
#include "stg34/scenery_art/entrance_units_part_1_polygons.inc"
};
static Sint32 entrance_units_part_1_vertices[99] = {
#include "stg34/scenery_art/entrance_units_part_1_vertices.inc"
};
static NJS_CNK_MODEL entrance_units_part_1_model = {
#include "stg34/scenery_art/entrance_units_part_1_model.inc"
};
static NJS_CNK_OBJECT entrance_units_part_1 = {
#include "stg34/scenery_art/entrance_units_part_1.inc"
};
NJS_CNK_OBJECT entrance_units = {
#include "stg34/scenery_art/entrance_units.inc"
};
static Sint16 entrance_flames_part_8_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_8_polygons.inc"
};
static Sint32 entrance_flames_part_8_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_8_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_8_model = {
#include "stg34/scenery_art/entrance_flames_part_8_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_8 = {
#include "stg34/scenery_art/entrance_flames_part_8.inc"
};
static Sint16 entrance_flames_part_7_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_7_polygons.inc"
};
static Sint32 entrance_flames_part_7_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_7_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_7_model = {
#include "stg34/scenery_art/entrance_flames_part_7_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_7 = {
#include "stg34/scenery_art/entrance_flames_part_7.inc"
};
static Sint16 entrance_flames_part_6_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_6_polygons.inc"
};
static Sint32 entrance_flames_part_6_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_6_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_6_model = {
#include "stg34/scenery_art/entrance_flames_part_6_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_6 = {
#include "stg34/scenery_art/entrance_flames_part_6.inc"
};
static Sint16 entrance_flames_part_5_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_5_polygons.inc"
};
static Sint32 entrance_flames_part_5_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_5_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_5_model = {
#include "stg34/scenery_art/entrance_flames_part_5_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_5 = {
#include "stg34/scenery_art/entrance_flames_part_5.inc"
};
static Sint16 entrance_flames_part_4_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_4_polygons.inc"
};
static Sint32 entrance_flames_part_4_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_4_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_4_model = {
#include "stg34/scenery_art/entrance_flames_part_4_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_4 = {
#include "stg34/scenery_art/entrance_flames_part_4.inc"
};
static Sint16 entrance_flames_part_3_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_3_polygons.inc"
};
static Sint32 entrance_flames_part_3_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_3_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_3_model = {
#include "stg34/scenery_art/entrance_flames_part_3_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_3 = {
#include "stg34/scenery_art/entrance_flames_part_3.inc"
};
static Sint16 entrance_flames_part_2_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_2_polygons.inc"
};
static Sint32 entrance_flames_part_2_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_2_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_2_model = {
#include "stg34/scenery_art/entrance_flames_part_2_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_2 = {
#include "stg34/scenery_art/entrance_flames_part_2.inc"
};
static Sint16 entrance_flames_part_1_polygons[17] = {
#include "stg34/scenery_art/entrance_flames_part_1_polygons.inc"
};
static Sint32 entrance_flames_part_1_vertices[27] = {
#include "stg34/scenery_art/entrance_flames_part_1_vertices.inc"
};
static NJS_CNK_MODEL entrance_flames_part_1_model = {
#include "stg34/scenery_art/entrance_flames_part_1_model.inc"
};
static NJS_CNK_OBJECT entrance_flames_part_1 = {
#include "stg34/scenery_art/entrance_flames_part_1.inc"
};
NJS_CNK_OBJECT entrance_flames = {
#include "stg34/scenery_art/entrance_flames.inc"
};
static Sint16 left_flames_part_8_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_8_polygons.inc"
};
static Sint32 left_flames_part_8_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_8_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_8_model = {
#include "stg34/scenery_art/left_flames_part_8_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_8 = {
#include "stg34/scenery_art/left_flames_part_8.inc"
};
static Sint16 left_flames_part_7_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_7_polygons.inc"
};
static Sint32 left_flames_part_7_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_7_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_7_model = {
#include "stg34/scenery_art/left_flames_part_7_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_7 = {
#include "stg34/scenery_art/left_flames_part_7.inc"
};
static Sint16 left_flames_part_6_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_6_polygons.inc"
};
static Sint32 left_flames_part_6_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_6_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_6_model = {
#include "stg34/scenery_art/left_flames_part_6_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_6 = {
#include "stg34/scenery_art/left_flames_part_6.inc"
};
static Sint16 left_flames_part_5_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_5_polygons.inc"
};
static Sint32 left_flames_part_5_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_5_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_5_model = {
#include "stg34/scenery_art/left_flames_part_5_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_5 = {
#include "stg34/scenery_art/left_flames_part_5.inc"
};
static Sint16 left_flames_part_4_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_4_polygons.inc"
};
static Sint32 left_flames_part_4_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_4_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_4_model = {
#include "stg34/scenery_art/left_flames_part_4_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_4 = {
#include "stg34/scenery_art/left_flames_part_4.inc"
};
static Sint16 left_flames_part_3_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_3_polygons.inc"
};
static Sint32 left_flames_part_3_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_3_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_3_model = {
#include "stg34/scenery_art/left_flames_part_3_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_3 = {
#include "stg34/scenery_art/left_flames_part_3.inc"
};
static Sint16 left_flames_part_2_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_2_polygons.inc"
};
static Sint32 left_flames_part_2_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_2_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_2_model = {
#include "stg34/scenery_art/left_flames_part_2_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_2 = {
#include "stg34/scenery_art/left_flames_part_2.inc"
};
static Sint16 left_flames_part_1_polygons[17] = {
#include "stg34/scenery_art/left_flames_part_1_polygons.inc"
};
static Sint32 left_flames_part_1_vertices[27] = {
#include "stg34/scenery_art/left_flames_part_1_vertices.inc"
};
static NJS_CNK_MODEL left_flames_part_1_model = {
#include "stg34/scenery_art/left_flames_part_1_model.inc"
};
static NJS_CNK_OBJECT left_flames_part_1 = {
#include "stg34/scenery_art/left_flames_part_1.inc"
};
NJS_CNK_OBJECT left_flames = {
#include "stg34/scenery_art/left_flames.inc"
};
static Sint16 right_flames_part_8_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_8_polygons.inc"
};
static Sint32 right_flames_part_8_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_8_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_8_model = {
#include "stg34/scenery_art/right_flames_part_8_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_8 = {
#include "stg34/scenery_art/right_flames_part_8.inc"
};
static Sint16 right_flames_part_7_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_7_polygons.inc"
};
static Sint32 right_flames_part_7_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_7_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_7_model = {
#include "stg34/scenery_art/right_flames_part_7_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_7 = {
#include "stg34/scenery_art/right_flames_part_7.inc"
};
static Sint16 right_flames_part_6_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_6_polygons.inc"
};
static Sint32 right_flames_part_6_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_6_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_6_model = {
#include "stg34/scenery_art/right_flames_part_6_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_6 = {
#include "stg34/scenery_art/right_flames_part_6.inc"
};
static Sint16 right_flames_part_5_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_5_polygons.inc"
};
static Sint32 right_flames_part_5_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_5_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_5_model = {
#include "stg34/scenery_art/right_flames_part_5_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_5 = {
#include "stg34/scenery_art/right_flames_part_5.inc"
};
static Sint16 right_flames_part_4_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_4_polygons.inc"
};
static Sint32 right_flames_part_4_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_4_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_4_model = {
#include "stg34/scenery_art/right_flames_part_4_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_4 = {
#include "stg34/scenery_art/right_flames_part_4.inc"
};
static Sint16 right_flames_part_3_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_3_polygons.inc"
};
static Sint32 right_flames_part_3_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_3_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_3_model = {
#include "stg34/scenery_art/right_flames_part_3_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_3 = {
#include "stg34/scenery_art/right_flames_part_3.inc"
};
static Sint16 right_flames_part_2_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_2_polygons.inc"
};
static Sint32 right_flames_part_2_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_2_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_2_model = {
#include "stg34/scenery_art/right_flames_part_2_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_2 = {
#include "stg34/scenery_art/right_flames_part_2.inc"
};
static Sint16 right_flames_part_1_polygons[17] = {
#include "stg34/scenery_art/right_flames_part_1_polygons.inc"
};
static Sint32 right_flames_part_1_vertices[27] = {
#include "stg34/scenery_art/right_flames_part_1_vertices.inc"
};
static NJS_CNK_MODEL right_flames_part_1_model = {
#include "stg34/scenery_art/right_flames_part_1_model.inc"
};
static NJS_CNK_OBJECT right_flames_part_1 = {
#include "stg34/scenery_art/right_flames_part_1.inc"
};
NJS_CNK_OBJECT right_flames = {
#include "stg34/scenery_art/right_flames.inc"
};
static NJS_TEXNAME cylinder_lamp_textures_names[1] = {
#include "stg34/scenery_art/cylinder_lamp_textures_names.inc"
};
NJS_TEXLIST cylinder_lamp_textures = {
#include "stg34/scenery_art/cylinder_lamp_textures.inc"
};
static Sint16 cylinder_lamps_part_28_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_28_polygons.inc"
};
static Sint32 cylinder_lamps_part_28_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_28_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_28_model = {
#include "stg34/scenery_art/cylinder_lamps_part_28_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_28 = {
#include "stg34/scenery_art/cylinder_lamps_part_28.inc"
};
static Sint16 cylinder_lamps_part_27_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_27_polygons.inc"
};
static Sint32 cylinder_lamps_part_27_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_27_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_27_model = {
#include "stg34/scenery_art/cylinder_lamps_part_27_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_27 = {
#include "stg34/scenery_art/cylinder_lamps_part_27.inc"
};
static Sint16 cylinder_lamps_part_26_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_26_polygons.inc"
};
static Sint32 cylinder_lamps_part_26_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_26_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_26_model = {
#include "stg34/scenery_art/cylinder_lamps_part_26_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_26 = {
#include "stg34/scenery_art/cylinder_lamps_part_26.inc"
};
static Sint16 cylinder_lamps_part_25_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_25_polygons.inc"
};
static Sint32 cylinder_lamps_part_25_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_25_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_25_model = {
#include "stg34/scenery_art/cylinder_lamps_part_25_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_25 = {
#include "stg34/scenery_art/cylinder_lamps_part_25.inc"
};
static Sint16 cylinder_lamps_part_24_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_24_polygons.inc"
};
static Sint32 cylinder_lamps_part_24_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_24_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_24_model = {
#include "stg34/scenery_art/cylinder_lamps_part_24_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_24 = {
#include "stg34/scenery_art/cylinder_lamps_part_24.inc"
};
static Sint16 cylinder_lamps_part_23_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_23_polygons.inc"
};
static Sint32 cylinder_lamps_part_23_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_23_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_23_model = {
#include "stg34/scenery_art/cylinder_lamps_part_23_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_23 = {
#include "stg34/scenery_art/cylinder_lamps_part_23.inc"
};
static Sint16 cylinder_lamps_part_22_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_22_polygons.inc"
};
static Sint32 cylinder_lamps_part_22_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_22_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_22_model = {
#include "stg34/scenery_art/cylinder_lamps_part_22_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_22 = {
#include "stg34/scenery_art/cylinder_lamps_part_22.inc"
};
static Sint16 cylinder_lamps_part_21_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_21_polygons.inc"
};
static Sint32 cylinder_lamps_part_21_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_21_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_21_model = {
#include "stg34/scenery_art/cylinder_lamps_part_21_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_21 = {
#include "stg34/scenery_art/cylinder_lamps_part_21.inc"
};
static Sint16 cylinder_lamps_part_20_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_20_polygons.inc"
};
static Sint32 cylinder_lamps_part_20_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_20_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_20_model = {
#include "stg34/scenery_art/cylinder_lamps_part_20_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_20 = {
#include "stg34/scenery_art/cylinder_lamps_part_20.inc"
};
static Sint16 cylinder_lamps_part_19_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_19_polygons.inc"
};
static Sint32 cylinder_lamps_part_19_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_19_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_19_model = {
#include "stg34/scenery_art/cylinder_lamps_part_19_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_19 = {
#include "stg34/scenery_art/cylinder_lamps_part_19.inc"
};
static Sint16 cylinder_lamps_part_18_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_18_polygons.inc"
};
static Sint32 cylinder_lamps_part_18_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_18_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_18_model = {
#include "stg34/scenery_art/cylinder_lamps_part_18_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_18 = {
#include "stg34/scenery_art/cylinder_lamps_part_18.inc"
};
static Sint16 cylinder_lamps_part_17_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_17_polygons.inc"
};
static Sint32 cylinder_lamps_part_17_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_17_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_17_model = {
#include "stg34/scenery_art/cylinder_lamps_part_17_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_17 = {
#include "stg34/scenery_art/cylinder_lamps_part_17.inc"
};
static Sint16 cylinder_lamps_part_16_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_16_polygons.inc"
};
static Sint32 cylinder_lamps_part_16_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_16_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_16_model = {
#include "stg34/scenery_art/cylinder_lamps_part_16_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_16 = {
#include "stg34/scenery_art/cylinder_lamps_part_16.inc"
};
static Sint16 cylinder_lamps_part_15_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_15_polygons.inc"
};
static Sint32 cylinder_lamps_part_15_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_15_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_15_model = {
#include "stg34/scenery_art/cylinder_lamps_part_15_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_15 = {
#include "stg34/scenery_art/cylinder_lamps_part_15.inc"
};
static Sint16 cylinder_lamps_part_14_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_14_polygons.inc"
};
static Sint32 cylinder_lamps_part_14_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_14_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_14_model = {
#include "stg34/scenery_art/cylinder_lamps_part_14_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_14 = {
#include "stg34/scenery_art/cylinder_lamps_part_14.inc"
};
static Sint16 cylinder_lamps_part_13_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_13_polygons.inc"
};
static Sint32 cylinder_lamps_part_13_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_13_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_13_model = {
#include "stg34/scenery_art/cylinder_lamps_part_13_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_13 = {
#include "stg34/scenery_art/cylinder_lamps_part_13.inc"
};
static Sint16 cylinder_lamps_part_12_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_12_polygons.inc"
};
static Sint32 cylinder_lamps_part_12_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_12_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_12_model = {
#include "stg34/scenery_art/cylinder_lamps_part_12_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_12 = {
#include "stg34/scenery_art/cylinder_lamps_part_12.inc"
};
static Sint16 cylinder_lamps_part_11_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_11_polygons.inc"
};
static Sint32 cylinder_lamps_part_11_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_11_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_11_model = {
#include "stg34/scenery_art/cylinder_lamps_part_11_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_11 = {
#include "stg34/scenery_art/cylinder_lamps_part_11.inc"
};
static Sint16 cylinder_lamps_part_10_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_10_polygons.inc"
};
static Sint32 cylinder_lamps_part_10_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_10_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_10_model = {
#include "stg34/scenery_art/cylinder_lamps_part_10_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_10 = {
#include "stg34/scenery_art/cylinder_lamps_part_10.inc"
};
static Sint16 cylinder_lamps_part_9_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_9_polygons.inc"
};
static Sint32 cylinder_lamps_part_9_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_9_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_9_model = {
#include "stg34/scenery_art/cylinder_lamps_part_9_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_9 = {
#include "stg34/scenery_art/cylinder_lamps_part_9.inc"
};
static Sint16 cylinder_lamps_part_8_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_8_polygons.inc"
};
static Sint32 cylinder_lamps_part_8_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_8_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_8_model = {
#include "stg34/scenery_art/cylinder_lamps_part_8_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_8 = {
#include "stg34/scenery_art/cylinder_lamps_part_8.inc"
};
static Sint16 cylinder_lamps_part_7_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_7_polygons.inc"
};
static Sint32 cylinder_lamps_part_7_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_7_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_7_model = {
#include "stg34/scenery_art/cylinder_lamps_part_7_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_7 = {
#include "stg34/scenery_art/cylinder_lamps_part_7.inc"
};
static Sint16 cylinder_lamps_part_6_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_6_polygons.inc"
};
static Sint32 cylinder_lamps_part_6_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_6_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_6_model = {
#include "stg34/scenery_art/cylinder_lamps_part_6_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_6 = {
#include "stg34/scenery_art/cylinder_lamps_part_6.inc"
};
static Sint16 cylinder_lamps_part_5_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_5_polygons.inc"
};
static Sint32 cylinder_lamps_part_5_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_5_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_5_model = {
#include "stg34/scenery_art/cylinder_lamps_part_5_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_5 = {
#include "stg34/scenery_art/cylinder_lamps_part_5.inc"
};
static Sint16 cylinder_lamps_part_4_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_4_polygons.inc"
};
static Sint32 cylinder_lamps_part_4_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_4_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_4_model = {
#include "stg34/scenery_art/cylinder_lamps_part_4_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_4 = {
#include "stg34/scenery_art/cylinder_lamps_part_4.inc"
};
static Sint16 cylinder_lamps_part_3_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_3_polygons.inc"
};
static Sint32 cylinder_lamps_part_3_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_3_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_3_model = {
#include "stg34/scenery_art/cylinder_lamps_part_3_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_3 = {
#include "stg34/scenery_art/cylinder_lamps_part_3.inc"
};
static Sint16 cylinder_lamps_part_2_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_2_polygons.inc"
};
static Sint32 cylinder_lamps_part_2_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_2_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_2_model = {
#include "stg34/scenery_art/cylinder_lamps_part_2_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_2 = {
#include "stg34/scenery_art/cylinder_lamps_part_2.inc"
};
static Sint16 cylinder_lamps_part_1_polygons[39] = {
#include "stg34/scenery_art/cylinder_lamps_part_1_polygons.inc"
};
static Sint32 cylinder_lamps_part_1_vertices[35] = {
#include "stg34/scenery_art/cylinder_lamps_part_1_vertices.inc"
};
static NJS_CNK_MODEL cylinder_lamps_part_1_model = {
#include "stg34/scenery_art/cylinder_lamps_part_1_model.inc"
};
static NJS_CNK_OBJECT cylinder_lamps_part_1 = {
#include "stg34/scenery_art/cylinder_lamps_part_1.inc"
};
NJS_CNK_OBJECT cylinder_lamps[1] = {
#include "stg34/scenery_art/cylinder_lamps.inc"
};
static NJS_TEXNAME cylinder_textures_names[2] = {
#include "stg34/scenery_art/cylinder_textures_names.inc"
};
NJS_TEXLIST cylinder_textures = {
#include "stg34/scenery_art/cylinder_textures.inc"
};
static Sint16 cylinder_body_part_6_polygons[187] = {
#include "stg34/scenery_art/cylinder_body_part_6_polygons.inc"
};
static Sint32 cylinder_body_part_6_vertices[99] = {
#include "stg34/scenery_art/cylinder_body_part_6_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_6_model = {
#include "stg34/scenery_art/cylinder_body_part_6_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_6 = {
#include "stg34/scenery_art/cylinder_body_part_6.inc"
};
static Sint16 cylinder_body_part_5_polygons[187] = {
#include "stg34/scenery_art/cylinder_body_part_5_polygons.inc"
};
static Sint32 cylinder_body_part_5_vertices[99] = {
#include "stg34/scenery_art/cylinder_body_part_5_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_5_model = {
#include "stg34/scenery_art/cylinder_body_part_5_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_5 = {
#include "stg34/scenery_art/cylinder_body_part_5.inc"
};
static Sint16 cylinder_body_part_4_polygons[187] = {
#include "stg34/scenery_art/cylinder_body_part_4_polygons.inc"
};
static Sint32 cylinder_body_part_4_vertices[99] = {
#include "stg34/scenery_art/cylinder_body_part_4_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_4_model = {
#include "stg34/scenery_art/cylinder_body_part_4_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_4 = {
#include "stg34/scenery_art/cylinder_body_part_4.inc"
};
static Sint16 cylinder_body_part_3_polygons[187] = {
#include "stg34/scenery_art/cylinder_body_part_3_polygons.inc"
};
static Sint32 cylinder_body_part_3_vertices[99] = {
#include "stg34/scenery_art/cylinder_body_part_3_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_3_model = {
#include "stg34/scenery_art/cylinder_body_part_3_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_3 = {
#include "stg34/scenery_art/cylinder_body_part_3.inc"
};
static Sint16 cylinder_body_part_2_polygons[911] = {
#include "stg34/scenery_art/cylinder_body_part_2_polygons.inc"
};
static Sint32 cylinder_body_part_2_vertices[551] = {
#include "stg34/scenery_art/cylinder_body_part_2_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_2_model = {
#include "stg34/scenery_art/cylinder_body_part_2_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_2 = {
#include "stg34/scenery_art/cylinder_body_part_2.inc"
};
static Sint16 cylinder_body_part_1_polygons[897] = {
#include "stg34/scenery_art/cylinder_body_part_1_polygons.inc"
};
static Sint32 cylinder_body_part_1_vertices[551] = {
#include "stg34/scenery_art/cylinder_body_part_1_vertices.inc"
};
static NJS_CNK_MODEL cylinder_body_part_1_model = {
#include "stg34/scenery_art/cylinder_body_part_1_model.inc"
};
static NJS_CNK_OBJECT cylinder_body_part_1 = {
#include "stg34/scenery_art/cylinder_body_part_1.inc"
};
NJS_CNK_OBJECT cylinder_body = {
#include "stg34/scenery_art/cylinder_body.inc"
};
static NJS_MKEY_F cylinder_motion_joint_0_position[2] = {
#include "stg34/scenery_art/cylinder_motion_joint_0_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_0_angles[2] = {
#include "stg34/scenery_art/cylinder_motion_joint_0_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_1_position[63] = {
#include "stg34/scenery_art/cylinder_motion_joint_1_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_1_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_1_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_2_position[63] = {
#include "stg34/scenery_art/cylinder_motion_joint_2_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_2_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_2_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_3_position[125] = {
#include "stg34/scenery_art/cylinder_motion_joint_3_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_3_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_3_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_4_position[125] = {
#include "stg34/scenery_art/cylinder_motion_joint_4_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_4_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_4_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_5_position[125] = {
#include "stg34/scenery_art/cylinder_motion_joint_5_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_5_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_5_angles.inc"
};
static NJS_MKEY_F cylinder_motion_joint_6_position[125] = {
#include "stg34/scenery_art/cylinder_motion_joint_6_position.inc"
};
static NJS_MKEY_A cylinder_motion_joint_6_angles[184] = {
#include "stg34/scenery_art/cylinder_motion_joint_6_angles.inc"
};
static NJS_MDATA3 cylinder_motion_tracks[7] = {
#include "stg34/scenery_art/cylinder_motion_tracks.inc"
};
NJS_MOTION cylinder_motion = {
#include "stg34/scenery_art/cylinder_motion.inc"
};
static NJS_TEXNAME entrance_unit_textures_names[1] = {
#include "stg34/scenery_art/entrance_unit_textures_names.inc"
};
NJS_TEXLIST entrance_unit_textures = {
#include "stg34/scenery_art/entrance_unit_textures.inc"
};
static Sint16 entrance_unit_frame_15_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_15_polygons.inc"
};
static Sint32 entrance_unit_frame_15_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_15_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_15_model = {
#include "stg34/scenery_art/entrance_unit_frame_15_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_15_object = {
#include "stg34/scenery_art/entrance_unit_frame_15_object.inc"
};
static Sint16 entrance_unit_frame_14_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_14_polygons.inc"
};
static Sint32 entrance_unit_frame_14_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_14_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_14_model = {
#include "stg34/scenery_art/entrance_unit_frame_14_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_14_object = {
#include "stg34/scenery_art/entrance_unit_frame_14_object.inc"
};
static Sint16 entrance_unit_frame_13_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_13_polygons.inc"
};
static Sint32 entrance_unit_frame_13_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_13_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_13_model = {
#include "stg34/scenery_art/entrance_unit_frame_13_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_13_object = {
#include "stg34/scenery_art/entrance_unit_frame_13_object.inc"
};
static Sint16 entrance_unit_frame_12_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_12_polygons.inc"
};
static Sint32 entrance_unit_frame_12_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_12_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_12_model = {
#include "stg34/scenery_art/entrance_unit_frame_12_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_12_object = {
#include "stg34/scenery_art/entrance_unit_frame_12_object.inc"
};
static Sint16 entrance_unit_frame_11_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_11_polygons.inc"
};
static Sint32 entrance_unit_frame_11_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_11_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_11_model = {
#include "stg34/scenery_art/entrance_unit_frame_11_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_11_object = {
#include "stg34/scenery_art/entrance_unit_frame_11_object.inc"
};
static Sint16 entrance_unit_frame_10_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_10_polygons.inc"
};
static Sint32 entrance_unit_frame_10_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_10_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_10_model = {
#include "stg34/scenery_art/entrance_unit_frame_10_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_10_object = {
#include "stg34/scenery_art/entrance_unit_frame_10_object.inc"
};
static Sint16 entrance_unit_frame_9_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_9_polygons.inc"
};
static Sint32 entrance_unit_frame_9_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_9_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_9_model = {
#include "stg34/scenery_art/entrance_unit_frame_9_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_9_object = {
#include "stg34/scenery_art/entrance_unit_frame_9_object.inc"
};
static Sint16 entrance_unit_frame_8_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_8_polygons.inc"
};
static Sint32 entrance_unit_frame_8_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_8_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_8_model = {
#include "stg34/scenery_art/entrance_unit_frame_8_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_8_object = {
#include "stg34/scenery_art/entrance_unit_frame_8_object.inc"
};
static Sint16 entrance_unit_frame_7_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_7_polygons.inc"
};
static Sint32 entrance_unit_frame_7_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_7_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_7_model = {
#include "stg34/scenery_art/entrance_unit_frame_7_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_7_object = {
#include "stg34/scenery_art/entrance_unit_frame_7_object.inc"
};
static Sint16 entrance_unit_frame_6_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_6_polygons.inc"
};
static Sint32 entrance_unit_frame_6_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_6_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_6_model = {
#include "stg34/scenery_art/entrance_unit_frame_6_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_6_object = {
#include "stg34/scenery_art/entrance_unit_frame_6_object.inc"
};
static Sint16 entrance_unit_frame_5_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_5_polygons.inc"
};
static Sint32 entrance_unit_frame_5_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_5_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_5_model = {
#include "stg34/scenery_art/entrance_unit_frame_5_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_5_object = {
#include "stg34/scenery_art/entrance_unit_frame_5_object.inc"
};
static Sint16 entrance_unit_frame_4_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_4_polygons.inc"
};
static Sint32 entrance_unit_frame_4_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_4_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_4_model = {
#include "stg34/scenery_art/entrance_unit_frame_4_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_4_object = {
#include "stg34/scenery_art/entrance_unit_frame_4_object.inc"
};
static Sint16 entrance_unit_frame_3_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_3_polygons.inc"
};
static Sint32 entrance_unit_frame_3_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_3_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_3_model = {
#include "stg34/scenery_art/entrance_unit_frame_3_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_3_object = {
#include "stg34/scenery_art/entrance_unit_frame_3_object.inc"
};
static Sint16 entrance_unit_frame_2_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_2_polygons.inc"
};
static Sint32 entrance_unit_frame_2_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_2_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_2_model = {
#include "stg34/scenery_art/entrance_unit_frame_2_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_2_object = {
#include "stg34/scenery_art/entrance_unit_frame_2_object.inc"
};
static Sint16 entrance_unit_frame_1_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_1_polygons.inc"
};
static Sint32 entrance_unit_frame_1_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_1_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_1_model = {
#include "stg34/scenery_art/entrance_unit_frame_1_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_1_object = {
#include "stg34/scenery_art/entrance_unit_frame_1_object.inc"
};
static Sint16 entrance_unit_frame_0_polygons[221] = {
#include "stg34/scenery_art/entrance_unit_frame_0_polygons.inc"
};
static Sint32 entrance_unit_frame_0_vertices[99] = {
#include "stg34/scenery_art/entrance_unit_frame_0_vertices.inc"
};
static NJS_CNK_MODEL entrance_unit_frame_0_model = {
#include "stg34/scenery_art/entrance_unit_frame_0_model.inc"
};
static NJS_CNK_OBJECT entrance_unit_frame_0_object = {
#include "stg34/scenery_art/entrance_unit_frame_0_object.inc"
};
static NJS_CNK_OBJECT entrance_unit_sequence_part_0 = {
#include "stg34/scenery_art/entrance_unit_sequence_part_0.inc"
};
static NJS_TEXNAME entrance_flame_textures_names[1] = {
#include "stg34/scenery_art/entrance_flame_textures_names.inc"
};
NJS_TEXLIST entrance_flame_textures = {
#include "stg34/scenery_art/entrance_flame_textures.inc"
};
static Sint16 entrance_flame_frame_48_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_48_polygons.inc"
};
static Sint32 entrance_flame_frame_48_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_48_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_48_model = {
#include "stg34/scenery_art/entrance_flame_frame_48_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_48_object = {
#include "stg34/scenery_art/entrance_flame_frame_48_object.inc"
};
static Sint16 entrance_flame_frame_47_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_47_polygons.inc"
};
static Sint32 entrance_flame_frame_47_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_47_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_47_model = {
#include "stg34/scenery_art/entrance_flame_frame_47_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_47_object = {
#include "stg34/scenery_art/entrance_flame_frame_47_object.inc"
};
static Sint16 entrance_flame_frame_46_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_46_polygons.inc"
};
static Sint32 entrance_flame_frame_46_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_46_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_46_model = {
#include "stg34/scenery_art/entrance_flame_frame_46_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_46_object = {
#include "stg34/scenery_art/entrance_flame_frame_46_object.inc"
};
static Sint16 entrance_flame_frame_45_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_45_polygons.inc"
};
static Sint32 entrance_flame_frame_45_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_45_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_45_model = {
#include "stg34/scenery_art/entrance_flame_frame_45_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_45_object = {
#include "stg34/scenery_art/entrance_flame_frame_45_object.inc"
};
static Sint16 entrance_flame_frame_44_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_44_polygons.inc"
};
static Sint32 entrance_flame_frame_44_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_44_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_44_model = {
#include "stg34/scenery_art/entrance_flame_frame_44_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_44_object = {
#include "stg34/scenery_art/entrance_flame_frame_44_object.inc"
};
static Sint16 entrance_flame_frame_43_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_43_polygons.inc"
};
static Sint32 entrance_flame_frame_43_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_43_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_43_model = {
#include "stg34/scenery_art/entrance_flame_frame_43_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_43_object = {
#include "stg34/scenery_art/entrance_flame_frame_43_object.inc"
};
static Sint16 entrance_flame_frame_42_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_42_polygons.inc"
};
static Sint32 entrance_flame_frame_42_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_42_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_42_model = {
#include "stg34/scenery_art/entrance_flame_frame_42_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_42_object = {
#include "stg34/scenery_art/entrance_flame_frame_42_object.inc"
};
static Sint16 entrance_flame_frame_41_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_41_polygons.inc"
};
static Sint32 entrance_flame_frame_41_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_41_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_41_model = {
#include "stg34/scenery_art/entrance_flame_frame_41_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_41_object = {
#include "stg34/scenery_art/entrance_flame_frame_41_object.inc"
};
static Sint16 entrance_flame_frame_40_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_40_polygons.inc"
};
static Sint32 entrance_flame_frame_40_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_40_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_40_model = {
#include "stg34/scenery_art/entrance_flame_frame_40_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_40_object = {
#include "stg34/scenery_art/entrance_flame_frame_40_object.inc"
};
static Sint16 entrance_flame_frame_39_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_39_polygons.inc"
};
static Sint32 entrance_flame_frame_39_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_39_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_39_model = {
#include "stg34/scenery_art/entrance_flame_frame_39_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_39_object = {
#include "stg34/scenery_art/entrance_flame_frame_39_object.inc"
};
static Sint16 entrance_flame_frame_38_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_38_polygons.inc"
};
static Sint32 entrance_flame_frame_38_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_38_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_38_model = {
#include "stg34/scenery_art/entrance_flame_frame_38_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_38_object = {
#include "stg34/scenery_art/entrance_flame_frame_38_object.inc"
};
static Sint16 entrance_flame_frame_37_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_37_polygons.inc"
};
static Sint32 entrance_flame_frame_37_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_37_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_37_model = {
#include "stg34/scenery_art/entrance_flame_frame_37_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_37_object = {
#include "stg34/scenery_art/entrance_flame_frame_37_object.inc"
};
static Sint16 entrance_flame_frame_36_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_36_polygons.inc"
};
static Sint32 entrance_flame_frame_36_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_36_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_36_model = {
#include "stg34/scenery_art/entrance_flame_frame_36_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_36_object = {
#include "stg34/scenery_art/entrance_flame_frame_36_object.inc"
};
static Sint16 entrance_flame_frame_35_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_35_polygons.inc"
};
static Sint32 entrance_flame_frame_35_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_35_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_35_model = {
#include "stg34/scenery_art/entrance_flame_frame_35_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_35_object = {
#include "stg34/scenery_art/entrance_flame_frame_35_object.inc"
};
static Sint16 entrance_flame_frame_34_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_34_polygons.inc"
};
static Sint32 entrance_flame_frame_34_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_34_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_34_model = {
#include "stg34/scenery_art/entrance_flame_frame_34_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_34_object = {
#include "stg34/scenery_art/entrance_flame_frame_34_object.inc"
};
static Sint16 entrance_flame_frame_33_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_33_polygons.inc"
};
static Sint32 entrance_flame_frame_33_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_33_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_33_model = {
#include "stg34/scenery_art/entrance_flame_frame_33_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_33_object = {
#include "stg34/scenery_art/entrance_flame_frame_33_object.inc"
};
static Sint16 entrance_flame_frame_32_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_32_polygons.inc"
};
static Sint32 entrance_flame_frame_32_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_32_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_32_model = {
#include "stg34/scenery_art/entrance_flame_frame_32_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_32_object = {
#include "stg34/scenery_art/entrance_flame_frame_32_object.inc"
};
static Sint16 entrance_flame_frame_31_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_31_polygons.inc"
};
static Sint32 entrance_flame_frame_31_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_31_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_31_model = {
#include "stg34/scenery_art/entrance_flame_frame_31_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_31_object = {
#include "stg34/scenery_art/entrance_flame_frame_31_object.inc"
};
static Sint16 entrance_flame_frame_30_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_30_polygons.inc"
};
static Sint32 entrance_flame_frame_30_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_30_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_30_model = {
#include "stg34/scenery_art/entrance_flame_frame_30_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_30_object = {
#include "stg34/scenery_art/entrance_flame_frame_30_object.inc"
};
static Sint16 entrance_flame_frame_29_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_29_polygons.inc"
};
static Sint32 entrance_flame_frame_29_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_29_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_29_model = {
#include "stg34/scenery_art/entrance_flame_frame_29_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_29_object = {
#include "stg34/scenery_art/entrance_flame_frame_29_object.inc"
};
static Sint16 entrance_flame_frame_28_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_28_polygons.inc"
};
static Sint32 entrance_flame_frame_28_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_28_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_28_model = {
#include "stg34/scenery_art/entrance_flame_frame_28_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_28_object = {
#include "stg34/scenery_art/entrance_flame_frame_28_object.inc"
};
static Sint16 entrance_flame_frame_27_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_27_polygons.inc"
};
static Sint32 entrance_flame_frame_27_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_27_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_27_model = {
#include "stg34/scenery_art/entrance_flame_frame_27_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_27_object = {
#include "stg34/scenery_art/entrance_flame_frame_27_object.inc"
};
static Sint16 entrance_flame_frame_26_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_26_polygons.inc"
};
static Sint32 entrance_flame_frame_26_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_26_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_26_model = {
#include "stg34/scenery_art/entrance_flame_frame_26_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_26_object = {
#include "stg34/scenery_art/entrance_flame_frame_26_object.inc"
};
static Sint16 entrance_flame_frame_25_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_25_polygons.inc"
};
static Sint32 entrance_flame_frame_25_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_25_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_25_model = {
#include "stg34/scenery_art/entrance_flame_frame_25_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_25_object = {
#include "stg34/scenery_art/entrance_flame_frame_25_object.inc"
};
static Sint16 entrance_flame_frame_24_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_24_polygons.inc"
};
static Sint32 entrance_flame_frame_24_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_24_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_24_model = {
#include "stg34/scenery_art/entrance_flame_frame_24_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_24_object = {
#include "stg34/scenery_art/entrance_flame_frame_24_object.inc"
};
static Sint16 entrance_flame_frame_23_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_23_polygons.inc"
};
static Sint32 entrance_flame_frame_23_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_23_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_23_model = {
#include "stg34/scenery_art/entrance_flame_frame_23_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_23_object = {
#include "stg34/scenery_art/entrance_flame_frame_23_object.inc"
};
static Sint16 entrance_flame_frame_22_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_22_polygons.inc"
};
static Sint32 entrance_flame_frame_22_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_22_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_22_model = {
#include "stg34/scenery_art/entrance_flame_frame_22_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_22_object = {
#include "stg34/scenery_art/entrance_flame_frame_22_object.inc"
};
static Sint16 entrance_flame_frame_21_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_21_polygons.inc"
};
static Sint32 entrance_flame_frame_21_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_21_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_21_model = {
#include "stg34/scenery_art/entrance_flame_frame_21_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_21_object = {
#include "stg34/scenery_art/entrance_flame_frame_21_object.inc"
};
static Sint16 entrance_flame_frame_20_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_20_polygons.inc"
};
static Sint32 entrance_flame_frame_20_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_20_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_20_model = {
#include "stg34/scenery_art/entrance_flame_frame_20_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_20_object = {
#include "stg34/scenery_art/entrance_flame_frame_20_object.inc"
};
static Sint16 entrance_flame_frame_19_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_19_polygons.inc"
};
static Sint32 entrance_flame_frame_19_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_19_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_19_model = {
#include "stg34/scenery_art/entrance_flame_frame_19_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_19_object = {
#include "stg34/scenery_art/entrance_flame_frame_19_object.inc"
};
static Sint16 entrance_flame_frame_18_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_18_polygons.inc"
};
static Sint32 entrance_flame_frame_18_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_18_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_18_model = {
#include "stg34/scenery_art/entrance_flame_frame_18_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_18_object = {
#include "stg34/scenery_art/entrance_flame_frame_18_object.inc"
};
static Sint16 entrance_flame_frame_17_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_17_polygons.inc"
};
static Sint32 entrance_flame_frame_17_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_17_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_17_model = {
#include "stg34/scenery_art/entrance_flame_frame_17_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_17_object = {
#include "stg34/scenery_art/entrance_flame_frame_17_object.inc"
};
static Sint16 entrance_flame_frame_16_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_16_polygons.inc"
};
static Sint32 entrance_flame_frame_16_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_16_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_16_model = {
#include "stg34/scenery_art/entrance_flame_frame_16_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_16_object = {
#include "stg34/scenery_art/entrance_flame_frame_16_object.inc"
};
static Sint16 entrance_flame_frame_15_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_15_polygons.inc"
};
static Sint32 entrance_flame_frame_15_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_15_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_15_model = {
#include "stg34/scenery_art/entrance_flame_frame_15_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_15_object = {
#include "stg34/scenery_art/entrance_flame_frame_15_object.inc"
};
static Sint16 entrance_flame_frame_14_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_14_polygons.inc"
};
static Sint32 entrance_flame_frame_14_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_14_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_14_model = {
#include "stg34/scenery_art/entrance_flame_frame_14_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_14_object = {
#include "stg34/scenery_art/entrance_flame_frame_14_object.inc"
};
static Sint16 entrance_flame_frame_13_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_13_polygons.inc"
};
static Sint32 entrance_flame_frame_13_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_13_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_13_model = {
#include "stg34/scenery_art/entrance_flame_frame_13_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_13_object = {
#include "stg34/scenery_art/entrance_flame_frame_13_object.inc"
};
static Sint16 entrance_flame_frame_12_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_12_polygons.inc"
};
static Sint32 entrance_flame_frame_12_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_12_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_12_model = {
#include "stg34/scenery_art/entrance_flame_frame_12_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_12_object = {
#include "stg34/scenery_art/entrance_flame_frame_12_object.inc"
};
static Sint16 entrance_flame_frame_11_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_11_polygons.inc"
};
static Sint32 entrance_flame_frame_11_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_11_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_11_model = {
#include "stg34/scenery_art/entrance_flame_frame_11_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_11_object = {
#include "stg34/scenery_art/entrance_flame_frame_11_object.inc"
};
static Sint16 entrance_flame_frame_10_polygons[31] = {
#include "stg34/scenery_art/entrance_flame_frame_10_polygons.inc"
};
static Sint32 entrance_flame_frame_10_vertices[27] = {
#include "stg34/scenery_art/entrance_flame_frame_10_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_10_model = {
#include "stg34/scenery_art/entrance_flame_frame_10_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_10_object = {
#include "stg34/scenery_art/entrance_flame_frame_10_object.inc"
};
static Sint16 entrance_flame_frame_9_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_9_polygons.inc"
};
static Sint32 entrance_flame_frame_9_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_9_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_9_model = {
#include "stg34/scenery_art/entrance_flame_frame_9_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_9_object = {
#include "stg34/scenery_art/entrance_flame_frame_9_object.inc"
};
static Sint16 entrance_flame_frame_8_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_8_polygons.inc"
};
static Sint32 entrance_flame_frame_8_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_8_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_8_model = {
#include "stg34/scenery_art/entrance_flame_frame_8_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_8_object = {
#include "stg34/scenery_art/entrance_flame_frame_8_object.inc"
};
static Sint16 entrance_flame_frame_7_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_7_polygons.inc"
};
static Sint32 entrance_flame_frame_7_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_7_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_7_model = {
#include "stg34/scenery_art/entrance_flame_frame_7_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_7_object = {
#include "stg34/scenery_art/entrance_flame_frame_7_object.inc"
};
static Sint16 entrance_flame_frame_6_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_6_polygons.inc"
};
static Sint32 entrance_flame_frame_6_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_6_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_6_model = {
#include "stg34/scenery_art/entrance_flame_frame_6_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_6_object = {
#include "stg34/scenery_art/entrance_flame_frame_6_object.inc"
};
static Sint16 entrance_flame_frame_5_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_5_polygons.inc"
};
static Sint32 entrance_flame_frame_5_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_5_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_5_model = {
#include "stg34/scenery_art/entrance_flame_frame_5_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_5_object = {
#include "stg34/scenery_art/entrance_flame_frame_5_object.inc"
};
static Sint16 entrance_flame_frame_4_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_4_polygons.inc"
};
static Sint32 entrance_flame_frame_4_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_4_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_4_model = {
#include "stg34/scenery_art/entrance_flame_frame_4_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_4_object = {
#include "stg34/scenery_art/entrance_flame_frame_4_object.inc"
};
static Sint16 entrance_flame_frame_3_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_3_polygons.inc"
};
static Sint32 entrance_flame_frame_3_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_3_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_3_model = {
#include "stg34/scenery_art/entrance_flame_frame_3_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_3_object = {
#include "stg34/scenery_art/entrance_flame_frame_3_object.inc"
};
static Sint16 entrance_flame_frame_2_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_2_polygons.inc"
};
static Sint32 entrance_flame_frame_2_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_2_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_2_model = {
#include "stg34/scenery_art/entrance_flame_frame_2_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_2_object = {
#include "stg34/scenery_art/entrance_flame_frame_2_object.inc"
};
static Sint16 entrance_flame_frame_1_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_1_polygons.inc"
};
static Sint32 entrance_flame_frame_1_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_1_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_1_model = {
#include "stg34/scenery_art/entrance_flame_frame_1_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_1_object = {
#include "stg34/scenery_art/entrance_flame_frame_1_object.inc"
};
static Sint16 entrance_flame_frame_0_polygons[25] = {
#include "stg34/scenery_art/entrance_flame_frame_0_polygons.inc"
};
static Sint32 entrance_flame_frame_0_vertices[19] = {
#include "stg34/scenery_art/entrance_flame_frame_0_vertices.inc"
};
static NJS_CNK_MODEL entrance_flame_frame_0_model = {
#include "stg34/scenery_art/entrance_flame_frame_0_model.inc"
};
static NJS_CNK_OBJECT entrance_flame_frame_0_object = {
#include "stg34/scenery_art/entrance_flame_frame_0_object.inc"
};
static NJS_CNK_OBJECT entrance_flame_sequence_part_0 = {
#include "stg34/scenery_art/entrance_flame_sequence_part_0.inc"
};
static Sint16 rotor_flames_part_4_polygons[31] = {
#include "stg34/scenery_art/rotor_flames_part_4_polygons.inc"
};
static Sint32 rotor_flames_part_4_vertices[75] = {
#include "stg34/scenery_art/rotor_flames_part_4_vertices.inc"
};
static NJS_CNK_MODEL rotor_flames_part_4_model = {
#include "stg34/scenery_art/rotor_flames_part_4_model.inc"
};
static NJS_CNK_OBJECT rotor_flames_part_4 = {
#include "stg34/scenery_art/rotor_flames_part_4.inc"
};
static Sint16 rotor_flames_part_3_polygons[31] = {
#include "stg34/scenery_art/rotor_flames_part_3_polygons.inc"
};
static Sint32 rotor_flames_part_3_vertices[75] = {
#include "stg34/scenery_art/rotor_flames_part_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_flames_part_3_model = {
#include "stg34/scenery_art/rotor_flames_part_3_model.inc"
};
static NJS_CNK_OBJECT rotor_flames_part_3 = {
#include "stg34/scenery_art/rotor_flames_part_3.inc"
};
static Sint16 rotor_flames_part_2_polygons[31] = {
#include "stg34/scenery_art/rotor_flames_part_2_polygons.inc"
};
static Sint32 rotor_flames_part_2_vertices[75] = {
#include "stg34/scenery_art/rotor_flames_part_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_flames_part_2_model = {
#include "stg34/scenery_art/rotor_flames_part_2_model.inc"
};
static NJS_CNK_OBJECT rotor_flames_part_2 = {
#include "stg34/scenery_art/rotor_flames_part_2.inc"
};
static Sint16 rotor_flames_part_1_polygons[31] = {
#include "stg34/scenery_art/rotor_flames_part_1_polygons.inc"
};
static Sint32 rotor_flames_part_1_vertices[75] = {
#include "stg34/scenery_art/rotor_flames_part_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_flames_part_1_model = {
#include "stg34/scenery_art/rotor_flames_part_1_model.inc"
};
static NJS_CNK_OBJECT rotor_flames_part_1 = {
#include "stg34/scenery_art/rotor_flames_part_1.inc"
};
NJS_CNK_OBJECT rotor_flames = {
#include "stg34/scenery_art/rotor_flames.inc"
};
static Sint16 rotor_walls_part_2_polygons[111] = {
#include "stg34/scenery_art/rotor_walls_part_2_polygons.inc"
};
static Sint32 rotor_walls_part_2_vertices[387] = {
#include "stg34/scenery_art/rotor_walls_part_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_walls_part_2_model = {
#include "stg34/scenery_art/rotor_walls_part_2_model.inc"
};
static NJS_CNK_OBJECT rotor_walls_part_2 = {
#include "stg34/scenery_art/rotor_walls_part_2.inc"
};
static Sint16 rotor_walls_part_1_polygons[111] = {
#include "stg34/scenery_art/rotor_walls_part_1_polygons.inc"
};
static Sint32 rotor_walls_part_1_vertices[387] = {
#include "stg34/scenery_art/rotor_walls_part_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_walls_part_1_model = {
#include "stg34/scenery_art/rotor_walls_part_1_model.inc"
};
static NJS_CNK_OBJECT rotor_walls_part_1 = {
#include "stg34/scenery_art/rotor_walls_part_1.inc"
};
NJS_CNK_OBJECT rotor_walls = {
#include "stg34/scenery_art/rotor_walls.inc"
};
static NJS_TEXNAME rotor_textures_names[2] = {
#include "stg34/scenery_art/rotor_textures_names.inc"
};
NJS_TEXLIST rotor_textures = {
#include "stg34/scenery_art/rotor_textures.inc"
};
static Sint16 rotor_body_object_part_1_polygons[45] = {
#include "stg34/scenery_art/rotor_body_object_part_1_polygons.inc"
};
static Sint32 rotor_body_object_part_1_vertices[35] = {
#include "stg34/scenery_art/rotor_body_object_part_1_vertices.inc"
};
NJS_CNK_MODEL rotor_highlight = {
#include "stg34/scenery_art/rotor_highlight.inc"
};
static NJS_CNK_OBJECT rotor_body_object_part_1 = {
#include "stg34/scenery_art/rotor_body_object_part_1.inc"
};
static Sint16 rotor_body_polygons[107] = {
#include "stg34/scenery_art/rotor_body_polygons.inc"
};
static Sint32 rotor_body_vertices[67] = {
#include "stg34/scenery_art/rotor_body_vertices.inc"
};
NJS_CNK_MODEL rotor_body = {
#include "stg34/scenery_art/rotor_body.inc"
};
static NJS_CNK_OBJECT rotor_body_object_part_0 = {
#include "stg34/scenery_art/rotor_body_object_part_0.inc"
};
static NJS_CNK_OBJECT rotor_body_sequence_part_0 = {
#include "stg34/scenery_art/rotor_body_sequence_part_0.inc"
};
static NJS_TEXNAME rotor_flame_textures_names[1] = {
#include "stg34/scenery_art/rotor_flame_textures_names.inc"
};
NJS_TEXLIST rotor_flame_textures = {
#include "stg34/scenery_art/rotor_flame_textures.inc"
};
static Sint16 rotor_flame_frame_78_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_78_polygons.inc"
};
static Sint32 rotor_flame_frame_78_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_78_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_78_model = {
#include "stg34/scenery_art/rotor_flame_frame_78_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_78_object = {
#include "stg34/scenery_art/rotor_flame_frame_78_object.inc"
};
static Sint16 rotor_flame_frame_77_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_77_polygons.inc"
};
static Sint32 rotor_flame_frame_77_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_77_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_77_model = {
#include "stg34/scenery_art/rotor_flame_frame_77_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_77_object = {
#include "stg34/scenery_art/rotor_flame_frame_77_object.inc"
};
static Sint16 rotor_flame_frame_76_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_76_polygons.inc"
};
static Sint32 rotor_flame_frame_76_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_76_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_76_model = {
#include "stg34/scenery_art/rotor_flame_frame_76_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_76_object = {
#include "stg34/scenery_art/rotor_flame_frame_76_object.inc"
};
static Sint16 rotor_flame_frame_75_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_75_polygons.inc"
};
static Sint32 rotor_flame_frame_75_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_75_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_75_model = {
#include "stg34/scenery_art/rotor_flame_frame_75_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_75_object = {
#include "stg34/scenery_art/rotor_flame_frame_75_object.inc"
};
static Sint16 rotor_flame_frame_74_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_74_polygons.inc"
};
static Sint32 rotor_flame_frame_74_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_74_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_74_model = {
#include "stg34/scenery_art/rotor_flame_frame_74_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_74_object = {
#include "stg34/scenery_art/rotor_flame_frame_74_object.inc"
};
static Sint16 rotor_flame_frame_73_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_73_polygons.inc"
};
static Sint32 rotor_flame_frame_73_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_73_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_73_model = {
#include "stg34/scenery_art/rotor_flame_frame_73_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_73_object = {
#include "stg34/scenery_art/rotor_flame_frame_73_object.inc"
};
static Sint16 rotor_flame_frame_72_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_72_polygons.inc"
};
static Sint32 rotor_flame_frame_72_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_72_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_72_model = {
#include "stg34/scenery_art/rotor_flame_frame_72_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_72_object = {
#include "stg34/scenery_art/rotor_flame_frame_72_object.inc"
};
static Sint16 rotor_flame_frame_71_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_71_polygons.inc"
};
static Sint32 rotor_flame_frame_71_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_71_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_71_model = {
#include "stg34/scenery_art/rotor_flame_frame_71_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_71_object = {
#include "stg34/scenery_art/rotor_flame_frame_71_object.inc"
};
static Sint16 rotor_flame_frame_70_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_70_polygons.inc"
};
static Sint32 rotor_flame_frame_70_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_70_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_70_model = {
#include "stg34/scenery_art/rotor_flame_frame_70_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_70_object = {
#include "stg34/scenery_art/rotor_flame_frame_70_object.inc"
};
static Sint16 rotor_flame_frame_69_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_69_polygons.inc"
};
static Sint32 rotor_flame_frame_69_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_69_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_69_model = {
#include "stg34/scenery_art/rotor_flame_frame_69_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_69_object = {
#include "stg34/scenery_art/rotor_flame_frame_69_object.inc"
};
static Sint16 rotor_flame_frame_68_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_68_polygons.inc"
};
static Sint32 rotor_flame_frame_68_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_68_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_68_model = {
#include "stg34/scenery_art/rotor_flame_frame_68_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_68_object = {
#include "stg34/scenery_art/rotor_flame_frame_68_object.inc"
};
static Sint16 rotor_flame_frame_67_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_67_polygons.inc"
};
static Sint32 rotor_flame_frame_67_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_67_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_67_model = {
#include "stg34/scenery_art/rotor_flame_frame_67_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_67_object = {
#include "stg34/scenery_art/rotor_flame_frame_67_object.inc"
};
static Sint16 rotor_flame_frame_66_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_66_polygons.inc"
};
static Sint32 rotor_flame_frame_66_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_66_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_66_model = {
#include "stg34/scenery_art/rotor_flame_frame_66_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_66_object = {
#include "stg34/scenery_art/rotor_flame_frame_66_object.inc"
};
static Sint16 rotor_flame_frame_65_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_65_polygons.inc"
};
static Sint32 rotor_flame_frame_65_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_65_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_65_model = {
#include "stg34/scenery_art/rotor_flame_frame_65_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_65_object = {
#include "stg34/scenery_art/rotor_flame_frame_65_object.inc"
};
static Sint16 rotor_flame_frame_64_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_64_polygons.inc"
};
static Sint32 rotor_flame_frame_64_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_64_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_64_model = {
#include "stg34/scenery_art/rotor_flame_frame_64_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_64_object = {
#include "stg34/scenery_art/rotor_flame_frame_64_object.inc"
};
static Sint16 rotor_flame_frame_63_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_63_polygons.inc"
};
static Sint32 rotor_flame_frame_63_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_63_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_63_model = {
#include "stg34/scenery_art/rotor_flame_frame_63_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_63_object = {
#include "stg34/scenery_art/rotor_flame_frame_63_object.inc"
};
static Sint16 rotor_flame_frame_62_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_62_polygons.inc"
};
static Sint32 rotor_flame_frame_62_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_62_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_62_model = {
#include "stg34/scenery_art/rotor_flame_frame_62_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_62_object = {
#include "stg34/scenery_art/rotor_flame_frame_62_object.inc"
};
static Sint16 rotor_flame_frame_61_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_61_polygons.inc"
};
static Sint32 rotor_flame_frame_61_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_61_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_61_model = {
#include "stg34/scenery_art/rotor_flame_frame_61_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_61_object = {
#include "stg34/scenery_art/rotor_flame_frame_61_object.inc"
};
static Sint16 rotor_flame_frame_60_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_60_polygons.inc"
};
static Sint32 rotor_flame_frame_60_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_60_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_60_model = {
#include "stg34/scenery_art/rotor_flame_frame_60_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_60_object = {
#include "stg34/scenery_art/rotor_flame_frame_60_object.inc"
};
static Sint16 rotor_flame_frame_59_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_59_polygons.inc"
};
static Sint32 rotor_flame_frame_59_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_59_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_59_model = {
#include "stg34/scenery_art/rotor_flame_frame_59_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_59_object = {
#include "stg34/scenery_art/rotor_flame_frame_59_object.inc"
};
static Sint16 rotor_flame_frame_58_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_58_polygons.inc"
};
static Sint32 rotor_flame_frame_58_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_58_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_58_model = {
#include "stg34/scenery_art/rotor_flame_frame_58_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_58_object = {
#include "stg34/scenery_art/rotor_flame_frame_58_object.inc"
};
static Sint16 rotor_flame_frame_57_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_57_polygons.inc"
};
static Sint32 rotor_flame_frame_57_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_57_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_57_model = {
#include "stg34/scenery_art/rotor_flame_frame_57_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_57_object = {
#include "stg34/scenery_art/rotor_flame_frame_57_object.inc"
};
static Sint16 rotor_flame_frame_56_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_56_polygons.inc"
};
static Sint32 rotor_flame_frame_56_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_56_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_56_model = {
#include "stg34/scenery_art/rotor_flame_frame_56_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_56_object = {
#include "stg34/scenery_art/rotor_flame_frame_56_object.inc"
};
static Sint16 rotor_flame_frame_55_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_55_polygons.inc"
};
static Sint32 rotor_flame_frame_55_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_55_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_55_model = {
#include "stg34/scenery_art/rotor_flame_frame_55_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_55_object = {
#include "stg34/scenery_art/rotor_flame_frame_55_object.inc"
};
static Sint16 rotor_flame_frame_54_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_54_polygons.inc"
};
static Sint32 rotor_flame_frame_54_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_54_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_54_model = {
#include "stg34/scenery_art/rotor_flame_frame_54_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_54_object = {
#include "stg34/scenery_art/rotor_flame_frame_54_object.inc"
};
static Sint16 rotor_flame_frame_53_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_53_polygons.inc"
};
static Sint32 rotor_flame_frame_53_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_53_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_53_model = {
#include "stg34/scenery_art/rotor_flame_frame_53_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_53_object = {
#include "stg34/scenery_art/rotor_flame_frame_53_object.inc"
};
static Sint16 rotor_flame_frame_52_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_52_polygons.inc"
};
static Sint32 rotor_flame_frame_52_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_52_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_52_model = {
#include "stg34/scenery_art/rotor_flame_frame_52_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_52_object = {
#include "stg34/scenery_art/rotor_flame_frame_52_object.inc"
};
static Sint16 rotor_flame_frame_51_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_51_polygons.inc"
};
static Sint32 rotor_flame_frame_51_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_51_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_51_model = {
#include "stg34/scenery_art/rotor_flame_frame_51_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_51_object = {
#include "stg34/scenery_art/rotor_flame_frame_51_object.inc"
};
static Sint16 rotor_flame_frame_50_polygons[69] = {
#include "stg34/scenery_art/rotor_flame_frame_50_polygons.inc"
};
static Sint32 rotor_flame_frame_50_vertices[75] = {
#include "stg34/scenery_art/rotor_flame_frame_50_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_50_model = {
#include "stg34/scenery_art/rotor_flame_frame_50_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_50_object = {
#include "stg34/scenery_art/rotor_flame_frame_50_object.inc"
};
static Sint16 rotor_flame_frame_49_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_49_polygons.inc"
};
static Sint32 rotor_flame_frame_49_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_49_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_49_model = {
#include "stg34/scenery_art/rotor_flame_frame_49_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_49_object = {
#include "stg34/scenery_art/rotor_flame_frame_49_object.inc"
};
static Sint16 rotor_flame_frame_48_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_48_polygons.inc"
};
static Sint32 rotor_flame_frame_48_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_48_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_48_model = {
#include "stg34/scenery_art/rotor_flame_frame_48_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_48_object = {
#include "stg34/scenery_art/rotor_flame_frame_48_object.inc"
};
static Sint16 rotor_flame_frame_47_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_47_polygons.inc"
};
static Sint32 rotor_flame_frame_47_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_47_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_47_model = {
#include "stg34/scenery_art/rotor_flame_frame_47_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_47_object = {
#include "stg34/scenery_art/rotor_flame_frame_47_object.inc"
};
static Sint16 rotor_flame_frame_46_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_46_polygons.inc"
};
static Sint32 rotor_flame_frame_46_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_46_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_46_model = {
#include "stg34/scenery_art/rotor_flame_frame_46_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_46_object = {
#include "stg34/scenery_art/rotor_flame_frame_46_object.inc"
};
static Sint16 rotor_flame_frame_45_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_45_polygons.inc"
};
static Sint32 rotor_flame_frame_45_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_45_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_45_model = {
#include "stg34/scenery_art/rotor_flame_frame_45_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_45_object = {
#include "stg34/scenery_art/rotor_flame_frame_45_object.inc"
};
static Sint16 rotor_flame_frame_44_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_44_polygons.inc"
};
static Sint32 rotor_flame_frame_44_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_44_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_44_model = {
#include "stg34/scenery_art/rotor_flame_frame_44_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_44_object = {
#include "stg34/scenery_art/rotor_flame_frame_44_object.inc"
};
static Sint16 rotor_flame_frame_43_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_43_polygons.inc"
};
static Sint32 rotor_flame_frame_43_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_43_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_43_model = {
#include "stg34/scenery_art/rotor_flame_frame_43_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_43_object = {
#include "stg34/scenery_art/rotor_flame_frame_43_object.inc"
};
static Sint16 rotor_flame_frame_42_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_42_polygons.inc"
};
static Sint32 rotor_flame_frame_42_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_42_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_42_model = {
#include "stg34/scenery_art/rotor_flame_frame_42_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_42_object = {
#include "stg34/scenery_art/rotor_flame_frame_42_object.inc"
};
static Sint16 rotor_flame_frame_41_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_41_polygons.inc"
};
static Sint32 rotor_flame_frame_41_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_41_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_41_model = {
#include "stg34/scenery_art/rotor_flame_frame_41_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_41_object = {
#include "stg34/scenery_art/rotor_flame_frame_41_object.inc"
};
static Sint16 rotor_flame_frame_40_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_40_polygons.inc"
};
static Sint32 rotor_flame_frame_40_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_40_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_40_model = {
#include "stg34/scenery_art/rotor_flame_frame_40_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_40_object = {
#include "stg34/scenery_art/rotor_flame_frame_40_object.inc"
};
static Sint16 rotor_flame_frame_39_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_39_polygons.inc"
};
static Sint32 rotor_flame_frame_39_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_39_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_39_model = {
#include "stg34/scenery_art/rotor_flame_frame_39_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_39_object = {
#include "stg34/scenery_art/rotor_flame_frame_39_object.inc"
};
static Sint16 rotor_flame_frame_38_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_38_polygons.inc"
};
static Sint32 rotor_flame_frame_38_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_38_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_38_model = {
#include "stg34/scenery_art/rotor_flame_frame_38_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_38_object = {
#include "stg34/scenery_art/rotor_flame_frame_38_object.inc"
};
static Sint16 rotor_flame_frame_37_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_37_polygons.inc"
};
static Sint32 rotor_flame_frame_37_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_37_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_37_model = {
#include "stg34/scenery_art/rotor_flame_frame_37_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_37_object = {
#include "stg34/scenery_art/rotor_flame_frame_37_object.inc"
};
static Sint16 rotor_flame_frame_36_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_36_polygons.inc"
};
static Sint32 rotor_flame_frame_36_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_36_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_36_model = {
#include "stg34/scenery_art/rotor_flame_frame_36_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_36_object = {
#include "stg34/scenery_art/rotor_flame_frame_36_object.inc"
};
static Sint16 rotor_flame_frame_35_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_35_polygons.inc"
};
static Sint32 rotor_flame_frame_35_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_35_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_35_model = {
#include "stg34/scenery_art/rotor_flame_frame_35_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_35_object = {
#include "stg34/scenery_art/rotor_flame_frame_35_object.inc"
};
static Sint16 rotor_flame_frame_34_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_34_polygons.inc"
};
static Sint32 rotor_flame_frame_34_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_34_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_34_model = {
#include "stg34/scenery_art/rotor_flame_frame_34_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_34_object = {
#include "stg34/scenery_art/rotor_flame_frame_34_object.inc"
};
static Sint16 rotor_flame_frame_33_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_33_polygons.inc"
};
static Sint32 rotor_flame_frame_33_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_33_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_33_model = {
#include "stg34/scenery_art/rotor_flame_frame_33_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_33_object = {
#include "stg34/scenery_art/rotor_flame_frame_33_object.inc"
};
static Sint16 rotor_flame_frame_32_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_32_polygons.inc"
};
static Sint32 rotor_flame_frame_32_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_32_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_32_model = {
#include "stg34/scenery_art/rotor_flame_frame_32_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_32_object = {
#include "stg34/scenery_art/rotor_flame_frame_32_object.inc"
};
static Sint16 rotor_flame_frame_31_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_31_polygons.inc"
};
static Sint32 rotor_flame_frame_31_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_31_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_31_model = {
#include "stg34/scenery_art/rotor_flame_frame_31_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_31_object = {
#include "stg34/scenery_art/rotor_flame_frame_31_object.inc"
};
static Sint16 rotor_flame_frame_30_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_30_polygons.inc"
};
static Sint32 rotor_flame_frame_30_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_30_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_30_model = {
#include "stg34/scenery_art/rotor_flame_frame_30_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_30_object = {
#include "stg34/scenery_art/rotor_flame_frame_30_object.inc"
};
static Sint16 rotor_flame_frame_29_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_29_polygons.inc"
};
static Sint32 rotor_flame_frame_29_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_29_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_29_model = {
#include "stg34/scenery_art/rotor_flame_frame_29_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_29_object = {
#include "stg34/scenery_art/rotor_flame_frame_29_object.inc"
};
static Sint16 rotor_flame_frame_28_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_28_polygons.inc"
};
static Sint32 rotor_flame_frame_28_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_28_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_28_model = {
#include "stg34/scenery_art/rotor_flame_frame_28_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_28_object = {
#include "stg34/scenery_art/rotor_flame_frame_28_object.inc"
};
static Sint16 rotor_flame_frame_27_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_27_polygons.inc"
};
static Sint32 rotor_flame_frame_27_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_27_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_27_model = {
#include "stg34/scenery_art/rotor_flame_frame_27_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_27_object = {
#include "stg34/scenery_art/rotor_flame_frame_27_object.inc"
};
static Sint16 rotor_flame_frame_26_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_26_polygons.inc"
};
static Sint32 rotor_flame_frame_26_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_26_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_26_model = {
#include "stg34/scenery_art/rotor_flame_frame_26_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_26_object = {
#include "stg34/scenery_art/rotor_flame_frame_26_object.inc"
};
static Sint16 rotor_flame_frame_25_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_25_polygons.inc"
};
static Sint32 rotor_flame_frame_25_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_25_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_25_model = {
#include "stg34/scenery_art/rotor_flame_frame_25_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_25_object = {
#include "stg34/scenery_art/rotor_flame_frame_25_object.inc"
};
static Sint16 rotor_flame_frame_24_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_24_polygons.inc"
};
static Sint32 rotor_flame_frame_24_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_24_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_24_model = {
#include "stg34/scenery_art/rotor_flame_frame_24_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_24_object = {
#include "stg34/scenery_art/rotor_flame_frame_24_object.inc"
};
static Sint16 rotor_flame_frame_23_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_23_polygons.inc"
};
static Sint32 rotor_flame_frame_23_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_23_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_23_model = {
#include "stg34/scenery_art/rotor_flame_frame_23_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_23_object = {
#include "stg34/scenery_art/rotor_flame_frame_23_object.inc"
};
static Sint16 rotor_flame_frame_22_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_22_polygons.inc"
};
static Sint32 rotor_flame_frame_22_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_22_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_22_model = {
#include "stg34/scenery_art/rotor_flame_frame_22_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_22_object = {
#include "stg34/scenery_art/rotor_flame_frame_22_object.inc"
};
static Sint16 rotor_flame_frame_21_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_21_polygons.inc"
};
static Sint32 rotor_flame_frame_21_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_21_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_21_model = {
#include "stg34/scenery_art/rotor_flame_frame_21_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_21_object = {
#include "stg34/scenery_art/rotor_flame_frame_21_object.inc"
};
static Sint16 rotor_flame_frame_20_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_20_polygons.inc"
};
static Sint32 rotor_flame_frame_20_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_20_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_20_model = {
#include "stg34/scenery_art/rotor_flame_frame_20_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_20_object = {
#include "stg34/scenery_art/rotor_flame_frame_20_object.inc"
};
static Sint16 rotor_flame_frame_19_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_19_polygons.inc"
};
static Sint32 rotor_flame_frame_19_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_19_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_19_model = {
#include "stg34/scenery_art/rotor_flame_frame_19_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_19_object = {
#include "stg34/scenery_art/rotor_flame_frame_19_object.inc"
};
static Sint16 rotor_flame_frame_18_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_18_polygons.inc"
};
static Sint32 rotor_flame_frame_18_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_18_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_18_model = {
#include "stg34/scenery_art/rotor_flame_frame_18_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_18_object = {
#include "stg34/scenery_art/rotor_flame_frame_18_object.inc"
};
static Sint16 rotor_flame_frame_17_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_17_polygons.inc"
};
static Sint32 rotor_flame_frame_17_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_17_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_17_model = {
#include "stg34/scenery_art/rotor_flame_frame_17_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_17_object = {
#include "stg34/scenery_art/rotor_flame_frame_17_object.inc"
};
static Sint16 rotor_flame_frame_16_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_16_polygons.inc"
};
static Sint32 rotor_flame_frame_16_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_16_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_16_model = {
#include "stg34/scenery_art/rotor_flame_frame_16_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_16_object = {
#include "stg34/scenery_art/rotor_flame_frame_16_object.inc"
};
static Sint16 rotor_flame_frame_15_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_15_polygons.inc"
};
static Sint32 rotor_flame_frame_15_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_15_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_15_model = {
#include "stg34/scenery_art/rotor_flame_frame_15_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_15_object = {
#include "stg34/scenery_art/rotor_flame_frame_15_object.inc"
};
static Sint16 rotor_flame_frame_14_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_14_polygons.inc"
};
static Sint32 rotor_flame_frame_14_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_14_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_14_model = {
#include "stg34/scenery_art/rotor_flame_frame_14_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_14_object = {
#include "stg34/scenery_art/rotor_flame_frame_14_object.inc"
};
static Sint16 rotor_flame_frame_13_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_13_polygons.inc"
};
static Sint32 rotor_flame_frame_13_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_13_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_13_model = {
#include "stg34/scenery_art/rotor_flame_frame_13_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_13_object = {
#include "stg34/scenery_art/rotor_flame_frame_13_object.inc"
};
static Sint16 rotor_flame_frame_12_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_12_polygons.inc"
};
static Sint32 rotor_flame_frame_12_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_12_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_12_model = {
#include "stg34/scenery_art/rotor_flame_frame_12_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_12_object = {
#include "stg34/scenery_art/rotor_flame_frame_12_object.inc"
};
static Sint16 rotor_flame_frame_11_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_11_polygons.inc"
};
static Sint32 rotor_flame_frame_11_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_11_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_11_model = {
#include "stg34/scenery_art/rotor_flame_frame_11_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_11_object = {
#include "stg34/scenery_art/rotor_flame_frame_11_object.inc"
};
static Sint16 rotor_flame_frame_10_polygons[51] = {
#include "stg34/scenery_art/rotor_flame_frame_10_polygons.inc"
};
static Sint32 rotor_flame_frame_10_vertices[51] = {
#include "stg34/scenery_art/rotor_flame_frame_10_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_10_model = {
#include "stg34/scenery_art/rotor_flame_frame_10_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_10_object = {
#include "stg34/scenery_art/rotor_flame_frame_10_object.inc"
};
static Sint16 rotor_flame_frame_9_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_9_polygons.inc"
};
static Sint32 rotor_flame_frame_9_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_9_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_9_model = {
#include "stg34/scenery_art/rotor_flame_frame_9_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_9_object = {
#include "stg34/scenery_art/rotor_flame_frame_9_object.inc"
};
static Sint16 rotor_flame_frame_8_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_8_polygons.inc"
};
static Sint32 rotor_flame_frame_8_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_8_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_8_model = {
#include "stg34/scenery_art/rotor_flame_frame_8_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_8_object = {
#include "stg34/scenery_art/rotor_flame_frame_8_object.inc"
};
static Sint16 rotor_flame_frame_7_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_7_polygons.inc"
};
static Sint32 rotor_flame_frame_7_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_7_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_7_model = {
#include "stg34/scenery_art/rotor_flame_frame_7_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_7_object = {
#include "stg34/scenery_art/rotor_flame_frame_7_object.inc"
};
static Sint16 rotor_flame_frame_6_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_6_polygons.inc"
};
static Sint32 rotor_flame_frame_6_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_6_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_6_model = {
#include "stg34/scenery_art/rotor_flame_frame_6_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_6_object = {
#include "stg34/scenery_art/rotor_flame_frame_6_object.inc"
};
static Sint16 rotor_flame_frame_5_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_5_polygons.inc"
};
static Sint32 rotor_flame_frame_5_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_5_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_5_model = {
#include "stg34/scenery_art/rotor_flame_frame_5_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_5_object = {
#include "stg34/scenery_art/rotor_flame_frame_5_object.inc"
};
static Sint16 rotor_flame_frame_4_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_4_polygons.inc"
};
static Sint32 rotor_flame_frame_4_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_4_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_4_model = {
#include "stg34/scenery_art/rotor_flame_frame_4_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_4_object = {
#include "stg34/scenery_art/rotor_flame_frame_4_object.inc"
};
static Sint16 rotor_flame_frame_3_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_3_polygons.inc"
};
static Sint32 rotor_flame_frame_3_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_3_model = {
#include "stg34/scenery_art/rotor_flame_frame_3_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_3_object = {
#include "stg34/scenery_art/rotor_flame_frame_3_object.inc"
};
static Sint16 rotor_flame_frame_2_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_2_polygons.inc"
};
static Sint32 rotor_flame_frame_2_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_2_model = {
#include "stg34/scenery_art/rotor_flame_frame_2_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_2_object = {
#include "stg34/scenery_art/rotor_flame_frame_2_object.inc"
};
static Sint16 rotor_flame_frame_1_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_1_polygons.inc"
};
static Sint32 rotor_flame_frame_1_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_1_model = {
#include "stg34/scenery_art/rotor_flame_frame_1_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_1_object = {
#include "stg34/scenery_art/rotor_flame_frame_1_object.inc"
};
static Sint16 rotor_flame_frame_0_polygons[39] = {
#include "stg34/scenery_art/rotor_flame_frame_0_polygons.inc"
};
static Sint32 rotor_flame_frame_0_vertices[35] = {
#include "stg34/scenery_art/rotor_flame_frame_0_vertices.inc"
};
static NJS_CNK_MODEL rotor_flame_frame_0_model = {
#include "stg34/scenery_art/rotor_flame_frame_0_model.inc"
};
static NJS_CNK_OBJECT rotor_flame_frame_0_object = {
#include "stg34/scenery_art/rotor_flame_frame_0_object.inc"
};
static NJS_CNK_OBJECT rotor_flame_sequence_part_0 = {
#include "stg34/scenery_art/rotor_flame_sequence_part_0.inc"
};
static NJS_TEXNAME rotor_wall_textures_names[1] = {
#include "stg34/scenery_art/rotor_wall_textures_names.inc"
};
NJS_TEXLIST rotor_wall_textures = {
#include "stg34/scenery_art/rotor_wall_textures.inc"
};
static Sint16 rotor_wall_frame_7_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_7_polygons.inc"
};
static Sint32 rotor_wall_frame_7_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_7_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_7_model = {
#include "stg34/scenery_art/rotor_wall_frame_7_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_7_object = {
#include "stg34/scenery_art/rotor_wall_frame_7_object.inc"
};
static Sint16 rotor_wall_frame_6_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_6_polygons.inc"
};
static Sint32 rotor_wall_frame_6_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_6_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_6_model = {
#include "stg34/scenery_art/rotor_wall_frame_6_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_6_object = {
#include "stg34/scenery_art/rotor_wall_frame_6_object.inc"
};
static Sint16 rotor_wall_frame_5_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_5_polygons.inc"
};
static Sint32 rotor_wall_frame_5_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_5_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_5_model = {
#include "stg34/scenery_art/rotor_wall_frame_5_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_5_object = {
#include "stg34/scenery_art/rotor_wall_frame_5_object.inc"
};
static Sint16 rotor_wall_frame_4_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_4_polygons.inc"
};
static Sint32 rotor_wall_frame_4_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_4_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_4_model = {
#include "stg34/scenery_art/rotor_wall_frame_4_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_4_object = {
#include "stg34/scenery_art/rotor_wall_frame_4_object.inc"
};
static Sint16 rotor_wall_frame_3_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_3_polygons.inc"
};
static Sint32 rotor_wall_frame_3_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_3_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_3_model = {
#include "stg34/scenery_art/rotor_wall_frame_3_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_3_object = {
#include "stg34/scenery_art/rotor_wall_frame_3_object.inc"
};
static Sint16 rotor_wall_frame_2_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_2_polygons.inc"
};
static Sint32 rotor_wall_frame_2_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_2_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_2_model = {
#include "stg34/scenery_art/rotor_wall_frame_2_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_2_object = {
#include "stg34/scenery_art/rotor_wall_frame_2_object.inc"
};
static Sint16 rotor_wall_frame_1_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_1_polygons.inc"
};
static Sint32 rotor_wall_frame_1_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_1_model = {
#include "stg34/scenery_art/rotor_wall_frame_1_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_1_object = {
#include "stg34/scenery_art/rotor_wall_frame_1_object.inc"
};
static Sint16 rotor_wall_frame_0_polygons[585] = {
#include "stg34/scenery_art/rotor_wall_frame_0_polygons.inc"
};
static Sint32 rotor_wall_frame_0_vertices[387] = {
#include "stg34/scenery_art/rotor_wall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL rotor_wall_frame_0_model = {
#include "stg34/scenery_art/rotor_wall_frame_0_model.inc"
};
static NJS_CNK_OBJECT rotor_wall_frame_0_object = {
#include "stg34/scenery_art/rotor_wall_frame_0_object.inc"
};
static NJS_CNK_OBJECT rotor_wall_sequence_part_0 = {
#include "stg34/scenery_art/rotor_wall_sequence_part_0.inc"
};
static Sint16 shaft_flames_part_1_polygons[95] = {
#include "stg34/scenery_art/shaft_flames_part_1_polygons.inc"
};
static Sint32 shaft_flames_part_1_vertices[291] = {
#include "stg34/scenery_art/shaft_flames_part_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_flames_part_1_model = {
#include "stg34/scenery_art/shaft_flames_part_1_model.inc"
};
static NJS_CNK_OBJECT shaft_flames_part_1 = {
#include "stg34/scenery_art/shaft_flames_part_1.inc"
};
NJS_CNK_OBJECT shaft_flames = {
#include "stg34/scenery_art/shaft_flames.inc"
};
static Sint16 shaft_units_part_1_polygons[101] = {
#include "stg34/scenery_art/shaft_units_part_1_polygons.inc"
};
static Sint32 shaft_units_part_1_vertices[315] = {
#include "stg34/scenery_art/shaft_units_part_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_units_part_1_model = {
#include "stg34/scenery_art/shaft_units_part_1_model.inc"
};
static NJS_CNK_OBJECT shaft_units_part_1 = {
#include "stg34/scenery_art/shaft_units_part_1.inc"
};
NJS_CNK_OBJECT shaft_units = {
#include "stg34/scenery_art/shaft_units.inc"
};
static NJS_TEXNAME flicker_lamp_textures_names[1] = {
#include "stg34/scenery_art/flicker_lamp_textures_names.inc"
};
NJS_TEXLIST flicker_lamp_textures = {
#include "stg34/scenery_art/flicker_lamp_textures.inc"
};
static Sint16 flicker_lamps_part_14_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_14_polygons.inc"
};
static Sint32 flicker_lamps_part_14_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_14_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_14_model = {
#include "stg34/scenery_art/flicker_lamps_part_14_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_14 = {
#include "stg34/scenery_art/flicker_lamps_part_14.inc"
};
static Sint16 flicker_lamps_part_13_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_13_polygons.inc"
};
static Sint32 flicker_lamps_part_13_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_13_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_13_model = {
#include "stg34/scenery_art/flicker_lamps_part_13_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_13 = {
#include "stg34/scenery_art/flicker_lamps_part_13.inc"
};
static Sint16 flicker_lamps_part_12_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_12_polygons.inc"
};
static Sint32 flicker_lamps_part_12_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_12_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_12_model = {
#include "stg34/scenery_art/flicker_lamps_part_12_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_12 = {
#include "stg34/scenery_art/flicker_lamps_part_12.inc"
};
static Sint16 flicker_lamps_part_11_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_11_polygons.inc"
};
static Sint32 flicker_lamps_part_11_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_11_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_11_model = {
#include "stg34/scenery_art/flicker_lamps_part_11_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_11 = {
#include "stg34/scenery_art/flicker_lamps_part_11.inc"
};
static Sint16 flicker_lamps_part_10_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_10_polygons.inc"
};
static Sint32 flicker_lamps_part_10_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_10_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_10_model = {
#include "stg34/scenery_art/flicker_lamps_part_10_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_10 = {
#include "stg34/scenery_art/flicker_lamps_part_10.inc"
};
static Sint16 flicker_lamps_part_9_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_9_polygons.inc"
};
static Sint32 flicker_lamps_part_9_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_9_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_9_model = {
#include "stg34/scenery_art/flicker_lamps_part_9_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_9 = {
#include "stg34/scenery_art/flicker_lamps_part_9.inc"
};
static Sint16 flicker_lamps_part_8_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_8_polygons.inc"
};
static Sint32 flicker_lamps_part_8_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_8_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_8_model = {
#include "stg34/scenery_art/flicker_lamps_part_8_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_8 = {
#include "stg34/scenery_art/flicker_lamps_part_8.inc"
};
static Sint16 flicker_lamps_part_7_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_7_polygons.inc"
};
static Sint32 flicker_lamps_part_7_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_7_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_7_model = {
#include "stg34/scenery_art/flicker_lamps_part_7_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_7 = {
#include "stg34/scenery_art/flicker_lamps_part_7.inc"
};
static Sint16 flicker_lamps_part_6_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_6_polygons.inc"
};
static Sint32 flicker_lamps_part_6_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_6_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_6_model = {
#include "stg34/scenery_art/flicker_lamps_part_6_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_6 = {
#include "stg34/scenery_art/flicker_lamps_part_6.inc"
};
static Sint16 flicker_lamps_part_5_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_5_polygons.inc"
};
static Sint32 flicker_lamps_part_5_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_5_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_5_model = {
#include "stg34/scenery_art/flicker_lamps_part_5_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_5 = {
#include "stg34/scenery_art/flicker_lamps_part_5.inc"
};
static Sint16 flicker_lamps_part_4_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_4_polygons.inc"
};
static Sint32 flicker_lamps_part_4_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_4_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_4_model = {
#include "stg34/scenery_art/flicker_lamps_part_4_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_4 = {
#include "stg34/scenery_art/flicker_lamps_part_4.inc"
};
static Sint16 flicker_lamps_part_3_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_3_polygons.inc"
};
static Sint32 flicker_lamps_part_3_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_3_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_3_model = {
#include "stg34/scenery_art/flicker_lamps_part_3_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_3 = {
#include "stg34/scenery_art/flicker_lamps_part_3.inc"
};
static Sint16 flicker_lamps_part_2_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_2_polygons.inc"
};
static Sint32 flicker_lamps_part_2_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_2_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_2_model = {
#include "stg34/scenery_art/flicker_lamps_part_2_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_2 = {
#include "stg34/scenery_art/flicker_lamps_part_2.inc"
};
static Sint16 flicker_lamps_part_1_polygons[39] = {
#include "stg34/scenery_art/flicker_lamps_part_1_polygons.inc"
};
static Sint32 flicker_lamps_part_1_vertices[35] = {
#include "stg34/scenery_art/flicker_lamps_part_1_vertices.inc"
};
static NJS_CNK_MODEL flicker_lamps_part_1_model = {
#include "stg34/scenery_art/flicker_lamps_part_1_model.inc"
};
static NJS_CNK_OBJECT flicker_lamps_part_1 = {
#include "stg34/scenery_art/flicker_lamps_part_1.inc"
};
NJS_CNK_OBJECT flicker_lamps[1] = {
#include "stg34/scenery_art/flicker_lamps.inc"
};
static NJS_TEXNAME shaft_flame_textures_names[1] = {
#include "stg34/scenery_art/shaft_flame_textures_names.inc"
};
NJS_TEXLIST shaft_flame_textures = {
#include "stg34/scenery_art/shaft_flame_textures.inc"
};
static Sint16 shaft_flame_frame_79_polygons[169] = {
#include "stg34/scenery_art/shaft_flame_frame_79_polygons.inc"
};
static Sint32 shaft_flame_frame_79_vertices[195] = {
#include "stg34/scenery_art/shaft_flame_frame_79_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_79_model = {
#include "stg34/scenery_art/shaft_flame_frame_79_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_79_object = {
#include "stg34/scenery_art/shaft_flame_frame_79_object.inc"
};
static Sint16 shaft_flame_frame_78_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_78_polygons.inc"
};
static Sint32 shaft_flame_frame_78_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_78_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_78_model = {
#include "stg34/scenery_art/shaft_flame_frame_78_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_78_object = {
#include "stg34/scenery_art/shaft_flame_frame_78_object.inc"
};
static Sint16 shaft_flame_frame_77_polygons[235] = {
#include "stg34/scenery_art/shaft_flame_frame_77_polygons.inc"
};
static Sint32 shaft_flame_frame_77_vertices[283] = {
#include "stg34/scenery_art/shaft_flame_frame_77_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_77_model = {
#include "stg34/scenery_art/shaft_flame_frame_77_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_77_object = {
#include "stg34/scenery_art/shaft_flame_frame_77_object.inc"
};
static Sint16 shaft_flame_frame_76_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_76_polygons.inc"
};
static Sint32 shaft_flame_frame_76_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_76_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_76_model = {
#include "stg34/scenery_art/shaft_flame_frame_76_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_76_object = {
#include "stg34/scenery_art/shaft_flame_frame_76_object.inc"
};
static Sint16 shaft_flame_frame_75_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_75_polygons.inc"
};
static Sint32 shaft_flame_frame_75_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_75_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_75_model = {
#include "stg34/scenery_art/shaft_flame_frame_75_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_75_object = {
#include "stg34/scenery_art/shaft_flame_frame_75_object.inc"
};
static Sint16 shaft_flame_frame_74_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_74_polygons.inc"
};
static Sint32 shaft_flame_frame_74_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_74_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_74_model = {
#include "stg34/scenery_art/shaft_flame_frame_74_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_74_object = {
#include "stg34/scenery_art/shaft_flame_frame_74_object.inc"
};
static Sint16 shaft_flame_frame_73_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_73_polygons.inc"
};
static Sint32 shaft_flame_frame_73_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_73_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_73_model = {
#include "stg34/scenery_art/shaft_flame_frame_73_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_73_object = {
#include "stg34/scenery_art/shaft_flame_frame_73_object.inc"
};
static Sint16 shaft_flame_frame_72_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_72_polygons.inc"
};
static Sint32 shaft_flame_frame_72_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_72_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_72_model = {
#include "stg34/scenery_art/shaft_flame_frame_72_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_72_object = {
#include "stg34/scenery_art/shaft_flame_frame_72_object.inc"
};
static Sint16 shaft_flame_frame_71_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_71_polygons.inc"
};
static Sint32 shaft_flame_frame_71_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_71_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_71_model = {
#include "stg34/scenery_art/shaft_flame_frame_71_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_71_object = {
#include "stg34/scenery_art/shaft_flame_frame_71_object.inc"
};
static Sint16 shaft_flame_frame_70_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_70_polygons.inc"
};
static Sint32 shaft_flame_frame_70_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_70_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_70_model = {
#include "stg34/scenery_art/shaft_flame_frame_70_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_70_object = {
#include "stg34/scenery_art/shaft_flame_frame_70_object.inc"
};
static Sint16 shaft_flame_frame_69_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_69_polygons.inc"
};
static Sint32 shaft_flame_frame_69_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_69_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_69_model = {
#include "stg34/scenery_art/shaft_flame_frame_69_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_69_object = {
#include "stg34/scenery_art/shaft_flame_frame_69_object.inc"
};
static Sint16 shaft_flame_frame_68_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_68_polygons.inc"
};
static Sint32 shaft_flame_frame_68_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_68_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_68_model = {
#include "stg34/scenery_art/shaft_flame_frame_68_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_68_object = {
#include "stg34/scenery_art/shaft_flame_frame_68_object.inc"
};
static Sint16 shaft_flame_frame_67_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_67_polygons.inc"
};
static Sint32 shaft_flame_frame_67_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_67_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_67_model = {
#include "stg34/scenery_art/shaft_flame_frame_67_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_67_object = {
#include "stg34/scenery_art/shaft_flame_frame_67_object.inc"
};
static Sint16 shaft_flame_frame_66_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_66_polygons.inc"
};
static Sint32 shaft_flame_frame_66_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_66_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_66_model = {
#include "stg34/scenery_art/shaft_flame_frame_66_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_66_object = {
#include "stg34/scenery_art/shaft_flame_frame_66_object.inc"
};
static Sint16 shaft_flame_frame_65_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_65_polygons.inc"
};
static Sint32 shaft_flame_frame_65_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_65_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_65_model = {
#include "stg34/scenery_art/shaft_flame_frame_65_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_65_object = {
#include "stg34/scenery_art/shaft_flame_frame_65_object.inc"
};
static Sint16 shaft_flame_frame_64_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_64_polygons.inc"
};
static Sint32 shaft_flame_frame_64_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_64_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_64_model = {
#include "stg34/scenery_art/shaft_flame_frame_64_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_64_object = {
#include "stg34/scenery_art/shaft_flame_frame_64_object.inc"
};
static Sint16 shaft_flame_frame_63_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_63_polygons.inc"
};
static Sint32 shaft_flame_frame_63_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_63_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_63_model = {
#include "stg34/scenery_art/shaft_flame_frame_63_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_63_object = {
#include "stg34/scenery_art/shaft_flame_frame_63_object.inc"
};
static Sint16 shaft_flame_frame_62_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_62_polygons.inc"
};
static Sint32 shaft_flame_frame_62_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_62_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_62_model = {
#include "stg34/scenery_art/shaft_flame_frame_62_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_62_object = {
#include "stg34/scenery_art/shaft_flame_frame_62_object.inc"
};
static Sint16 shaft_flame_frame_61_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_61_polygons.inc"
};
static Sint32 shaft_flame_frame_61_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_61_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_61_model = {
#include "stg34/scenery_art/shaft_flame_frame_61_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_61_object = {
#include "stg34/scenery_art/shaft_flame_frame_61_object.inc"
};
static Sint16 shaft_flame_frame_60_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_60_polygons.inc"
};
static Sint32 shaft_flame_frame_60_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_60_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_60_model = {
#include "stg34/scenery_art/shaft_flame_frame_60_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_60_object = {
#include "stg34/scenery_art/shaft_flame_frame_60_object.inc"
};
static Sint16 shaft_flame_frame_59_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_59_polygons.inc"
};
static Sint32 shaft_flame_frame_59_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_59_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_59_model = {
#include "stg34/scenery_art/shaft_flame_frame_59_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_59_object = {
#include "stg34/scenery_art/shaft_flame_frame_59_object.inc"
};
static Sint16 shaft_flame_frame_58_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_58_polygons.inc"
};
static Sint32 shaft_flame_frame_58_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_58_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_58_model = {
#include "stg34/scenery_art/shaft_flame_frame_58_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_58_object = {
#include "stg34/scenery_art/shaft_flame_frame_58_object.inc"
};
static Sint16 shaft_flame_frame_57_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_57_polygons.inc"
};
static Sint32 shaft_flame_frame_57_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_57_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_57_model = {
#include "stg34/scenery_art/shaft_flame_frame_57_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_57_object = {
#include "stg34/scenery_art/shaft_flame_frame_57_object.inc"
};
static Sint16 shaft_flame_frame_56_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_56_polygons.inc"
};
static Sint32 shaft_flame_frame_56_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_56_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_56_model = {
#include "stg34/scenery_art/shaft_flame_frame_56_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_56_object = {
#include "stg34/scenery_art/shaft_flame_frame_56_object.inc"
};
static Sint16 shaft_flame_frame_55_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_55_polygons.inc"
};
static Sint32 shaft_flame_frame_55_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_55_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_55_model = {
#include "stg34/scenery_art/shaft_flame_frame_55_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_55_object = {
#include "stg34/scenery_art/shaft_flame_frame_55_object.inc"
};
static Sint16 shaft_flame_frame_54_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_54_polygons.inc"
};
static Sint32 shaft_flame_frame_54_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_54_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_54_model = {
#include "stg34/scenery_art/shaft_flame_frame_54_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_54_object = {
#include "stg34/scenery_art/shaft_flame_frame_54_object.inc"
};
static Sint16 shaft_flame_frame_53_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_53_polygons.inc"
};
static Sint32 shaft_flame_frame_53_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_53_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_53_model = {
#include "stg34/scenery_art/shaft_flame_frame_53_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_53_object = {
#include "stg34/scenery_art/shaft_flame_frame_53_object.inc"
};
static Sint16 shaft_flame_frame_52_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_52_polygons.inc"
};
static Sint32 shaft_flame_frame_52_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_52_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_52_model = {
#include "stg34/scenery_art/shaft_flame_frame_52_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_52_object = {
#include "stg34/scenery_art/shaft_flame_frame_52_object.inc"
};
static Sint16 shaft_flame_frame_51_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_51_polygons.inc"
};
static Sint32 shaft_flame_frame_51_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_51_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_51_model = {
#include "stg34/scenery_art/shaft_flame_frame_51_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_51_object = {
#include "stg34/scenery_art/shaft_flame_frame_51_object.inc"
};
static Sint16 shaft_flame_frame_50_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_50_polygons.inc"
};
static Sint32 shaft_flame_frame_50_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_50_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_50_model = {
#include "stg34/scenery_art/shaft_flame_frame_50_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_50_object = {
#include "stg34/scenery_art/shaft_flame_frame_50_object.inc"
};
static Sint16 shaft_flame_frame_49_polygons[169] = {
#include "stg34/scenery_art/shaft_flame_frame_49_polygons.inc"
};
static Sint32 shaft_flame_frame_49_vertices[195] = {
#include "stg34/scenery_art/shaft_flame_frame_49_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_49_model = {
#include "stg34/scenery_art/shaft_flame_frame_49_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_49_object = {
#include "stg34/scenery_art/shaft_flame_frame_49_object.inc"
};
static Sint16 shaft_flame_frame_48_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_48_polygons.inc"
};
static Sint32 shaft_flame_frame_48_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_48_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_48_model = {
#include "stg34/scenery_art/shaft_flame_frame_48_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_48_object = {
#include "stg34/scenery_art/shaft_flame_frame_48_object.inc"
};
static Sint16 shaft_flame_frame_47_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_47_polygons.inc"
};
static Sint32 shaft_flame_frame_47_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_47_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_47_model = {
#include "stg34/scenery_art/shaft_flame_frame_47_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_47_object = {
#include "stg34/scenery_art/shaft_flame_frame_47_object.inc"
};
static Sint16 shaft_flame_frame_46_polygons[319] = {
#include "stg34/scenery_art/shaft_flame_frame_46_polygons.inc"
};
static Sint32 shaft_flame_frame_46_vertices[379] = {
#include "stg34/scenery_art/shaft_flame_frame_46_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_46_model = {
#include "stg34/scenery_art/shaft_flame_frame_46_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_46_object = {
#include "stg34/scenery_art/shaft_flame_frame_46_object.inc"
};
static Sint16 shaft_flame_frame_45_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_45_polygons.inc"
};
static Sint32 shaft_flame_frame_45_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_45_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_45_model = {
#include "stg34/scenery_art/shaft_flame_frame_45_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_45_object = {
#include "stg34/scenery_art/shaft_flame_frame_45_object.inc"
};
static Sint16 shaft_flame_frame_44_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_44_polygons.inc"
};
static Sint32 shaft_flame_frame_44_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_44_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_44_model = {
#include "stg34/scenery_art/shaft_flame_frame_44_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_44_object = {
#include "stg34/scenery_art/shaft_flame_frame_44_object.inc"
};
static Sint16 shaft_flame_frame_43_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_43_polygons.inc"
};
static Sint32 shaft_flame_frame_43_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_43_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_43_model = {
#include "stg34/scenery_art/shaft_flame_frame_43_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_43_object = {
#include "stg34/scenery_art/shaft_flame_frame_43_object.inc"
};
static Sint16 shaft_flame_frame_42_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_42_polygons.inc"
};
static Sint32 shaft_flame_frame_42_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_42_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_42_model = {
#include "stg34/scenery_art/shaft_flame_frame_42_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_42_object = {
#include "stg34/scenery_art/shaft_flame_frame_42_object.inc"
};
static Sint16 shaft_flame_frame_41_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_41_polygons.inc"
};
static Sint32 shaft_flame_frame_41_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_41_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_41_model = {
#include "stg34/scenery_art/shaft_flame_frame_41_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_41_object = {
#include "stg34/scenery_art/shaft_flame_frame_41_object.inc"
};
static Sint16 shaft_flame_frame_40_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_40_polygons.inc"
};
static Sint32 shaft_flame_frame_40_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_40_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_40_model = {
#include "stg34/scenery_art/shaft_flame_frame_40_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_40_object = {
#include "stg34/scenery_art/shaft_flame_frame_40_object.inc"
};
static Sint16 shaft_flame_frame_39_polygons[169] = {
#include "stg34/scenery_art/shaft_flame_frame_39_polygons.inc"
};
static Sint32 shaft_flame_frame_39_vertices[195] = {
#include "stg34/scenery_art/shaft_flame_frame_39_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_39_model = {
#include "stg34/scenery_art/shaft_flame_frame_39_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_39_object = {
#include "stg34/scenery_art/shaft_flame_frame_39_object.inc"
};
static Sint16 shaft_flame_frame_38_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_38_polygons.inc"
};
static Sint32 shaft_flame_frame_38_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_38_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_38_model = {
#include "stg34/scenery_art/shaft_flame_frame_38_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_38_object = {
#include "stg34/scenery_art/shaft_flame_frame_38_object.inc"
};
static Sint16 shaft_flame_frame_37_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_37_polygons.inc"
};
static Sint32 shaft_flame_frame_37_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_37_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_37_model = {
#include "stg34/scenery_art/shaft_flame_frame_37_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_37_object = {
#include "stg34/scenery_art/shaft_flame_frame_37_object.inc"
};
static Sint16 shaft_flame_frame_36_polygons[235] = {
#include "stg34/scenery_art/shaft_flame_frame_36_polygons.inc"
};
static Sint32 shaft_flame_frame_36_vertices[283] = {
#include "stg34/scenery_art/shaft_flame_frame_36_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_36_model = {
#include "stg34/scenery_art/shaft_flame_frame_36_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_36_object = {
#include "stg34/scenery_art/shaft_flame_frame_36_object.inc"
};
static Sint16 shaft_flame_frame_35_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_35_polygons.inc"
};
static Sint32 shaft_flame_frame_35_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_35_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_35_model = {
#include "stg34/scenery_art/shaft_flame_frame_35_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_35_object = {
#include "stg34/scenery_art/shaft_flame_frame_35_object.inc"
};
static Sint16 shaft_flame_frame_34_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_34_polygons.inc"
};
static Sint32 shaft_flame_frame_34_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_34_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_34_model = {
#include "stg34/scenery_art/shaft_flame_frame_34_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_34_object = {
#include "stg34/scenery_art/shaft_flame_frame_34_object.inc"
};
static Sint16 shaft_flame_frame_33_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_33_polygons.inc"
};
static Sint32 shaft_flame_frame_33_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_33_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_33_model = {
#include "stg34/scenery_art/shaft_flame_frame_33_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_33_object = {
#include "stg34/scenery_art/shaft_flame_frame_33_object.inc"
};
static Sint16 shaft_flame_frame_32_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_32_polygons.inc"
};
static Sint32 shaft_flame_frame_32_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_32_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_32_model = {
#include "stg34/scenery_art/shaft_flame_frame_32_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_32_object = {
#include "stg34/scenery_art/shaft_flame_frame_32_object.inc"
};
static Sint16 shaft_flame_frame_31_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_31_polygons.inc"
};
static Sint32 shaft_flame_frame_31_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_31_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_31_model = {
#include "stg34/scenery_art/shaft_flame_frame_31_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_31_object = {
#include "stg34/scenery_art/shaft_flame_frame_31_object.inc"
};
static Sint16 shaft_flame_frame_30_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_30_polygons.inc"
};
static Sint32 shaft_flame_frame_30_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_30_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_30_model = {
#include "stg34/scenery_art/shaft_flame_frame_30_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_30_object = {
#include "stg34/scenery_art/shaft_flame_frame_30_object.inc"
};
static Sint16 shaft_flame_frame_29_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_29_polygons.inc"
};
static Sint32 shaft_flame_frame_29_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_29_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_29_model = {
#include "stg34/scenery_art/shaft_flame_frame_29_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_29_object = {
#include "stg34/scenery_art/shaft_flame_frame_29_object.inc"
};
static Sint16 shaft_flame_frame_28_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_28_polygons.inc"
};
static Sint32 shaft_flame_frame_28_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_28_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_28_model = {
#include "stg34/scenery_art/shaft_flame_frame_28_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_28_object = {
#include "stg34/scenery_art/shaft_flame_frame_28_object.inc"
};
static Sint16 shaft_flame_frame_27_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_27_polygons.inc"
};
static Sint32 shaft_flame_frame_27_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_27_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_27_model = {
#include "stg34/scenery_art/shaft_flame_frame_27_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_27_object = {
#include "stg34/scenery_art/shaft_flame_frame_27_object.inc"
};
static Sint16 shaft_flame_frame_26_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_26_polygons.inc"
};
static Sint32 shaft_flame_frame_26_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_26_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_26_model = {
#include "stg34/scenery_art/shaft_flame_frame_26_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_26_object = {
#include "stg34/scenery_art/shaft_flame_frame_26_object.inc"
};
static Sint16 shaft_flame_frame_25_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_25_polygons.inc"
};
static Sint32 shaft_flame_frame_25_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_25_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_25_model = {
#include "stg34/scenery_art/shaft_flame_frame_25_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_25_object = {
#include "stg34/scenery_art/shaft_flame_frame_25_object.inc"
};
static Sint16 shaft_flame_frame_24_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_24_polygons.inc"
};
static Sint32 shaft_flame_frame_24_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_24_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_24_model = {
#include "stg34/scenery_art/shaft_flame_frame_24_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_24_object = {
#include "stg34/scenery_art/shaft_flame_frame_24_object.inc"
};
static Sint16 shaft_flame_frame_23_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_23_polygons.inc"
};
static Sint32 shaft_flame_frame_23_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_23_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_23_model = {
#include "stg34/scenery_art/shaft_flame_frame_23_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_23_object = {
#include "stg34/scenery_art/shaft_flame_frame_23_object.inc"
};
static Sint16 shaft_flame_frame_22_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_22_polygons.inc"
};
static Sint32 shaft_flame_frame_22_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_22_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_22_model = {
#include "stg34/scenery_art/shaft_flame_frame_22_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_22_object = {
#include "stg34/scenery_art/shaft_flame_frame_22_object.inc"
};
static Sint16 shaft_flame_frame_21_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_21_polygons.inc"
};
static Sint32 shaft_flame_frame_21_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_21_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_21_model = {
#include "stg34/scenery_art/shaft_flame_frame_21_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_21_object = {
#include "stg34/scenery_art/shaft_flame_frame_21_object.inc"
};
static Sint16 shaft_flame_frame_20_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_20_polygons.inc"
};
static Sint32 shaft_flame_frame_20_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_20_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_20_model = {
#include "stg34/scenery_art/shaft_flame_frame_20_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_20_object = {
#include "stg34/scenery_art/shaft_flame_frame_20_object.inc"
};
static Sint16 shaft_flame_frame_19_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_19_polygons.inc"
};
static Sint32 shaft_flame_frame_19_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_19_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_19_model = {
#include "stg34/scenery_art/shaft_flame_frame_19_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_19_object = {
#include "stg34/scenery_art/shaft_flame_frame_19_object.inc"
};
static Sint16 shaft_flame_frame_18_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_18_polygons.inc"
};
static Sint32 shaft_flame_frame_18_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_18_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_18_model = {
#include "stg34/scenery_art/shaft_flame_frame_18_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_18_object = {
#include "stg34/scenery_art/shaft_flame_frame_18_object.inc"
};
static Sint16 shaft_flame_frame_17_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_17_polygons.inc"
};
static Sint32 shaft_flame_frame_17_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_17_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_17_model = {
#include "stg34/scenery_art/shaft_flame_frame_17_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_17_object = {
#include "stg34/scenery_art/shaft_flame_frame_17_object.inc"
};
static Sint16 shaft_flame_frame_16_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_16_polygons.inc"
};
static Sint32 shaft_flame_frame_16_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_16_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_16_model = {
#include "stg34/scenery_art/shaft_flame_frame_16_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_16_object = {
#include "stg34/scenery_art/shaft_flame_frame_16_object.inc"
};
static Sint16 shaft_flame_frame_15_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_15_polygons.inc"
};
static Sint32 shaft_flame_frame_15_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_15_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_15_model = {
#include "stg34/scenery_art/shaft_flame_frame_15_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_15_object = {
#include "stg34/scenery_art/shaft_flame_frame_15_object.inc"
};
static Sint16 shaft_flame_frame_14_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_14_polygons.inc"
};
static Sint32 shaft_flame_frame_14_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_14_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_14_model = {
#include "stg34/scenery_art/shaft_flame_frame_14_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_14_object = {
#include "stg34/scenery_art/shaft_flame_frame_14_object.inc"
};
static Sint16 shaft_flame_frame_13_polygons[241] = {
#include "stg34/scenery_art/shaft_flame_frame_13_polygons.inc"
};
static Sint32 shaft_flame_frame_13_vertices[291] = {
#include "stg34/scenery_art/shaft_flame_frame_13_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_13_model = {
#include "stg34/scenery_art/shaft_flame_frame_13_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_13_object = {
#include "stg34/scenery_art/shaft_flame_frame_13_object.inc"
};
static Sint16 shaft_flame_frame_12_polygons[235] = {
#include "stg34/scenery_art/shaft_flame_frame_12_polygons.inc"
};
static Sint32 shaft_flame_frame_12_vertices[283] = {
#include "stg34/scenery_art/shaft_flame_frame_12_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_12_model = {
#include "stg34/scenery_art/shaft_flame_frame_12_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_12_object = {
#include "stg34/scenery_art/shaft_flame_frame_12_object.inc"
};
static Sint16 shaft_flame_frame_11_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_11_polygons.inc"
};
static Sint32 shaft_flame_frame_11_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_11_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_11_model = {
#include "stg34/scenery_art/shaft_flame_frame_11_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_11_object = {
#include "stg34/scenery_art/shaft_flame_frame_11_object.inc"
};
static Sint16 shaft_flame_frame_10_polygons[229] = {
#include "stg34/scenery_art/shaft_flame_frame_10_polygons.inc"
};
static Sint32 shaft_flame_frame_10_vertices[275] = {
#include "stg34/scenery_art/shaft_flame_frame_10_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_10_model = {
#include "stg34/scenery_art/shaft_flame_frame_10_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_10_object = {
#include "stg34/scenery_art/shaft_flame_frame_10_object.inc"
};
static Sint16 shaft_flame_frame_9_polygons[169] = {
#include "stg34/scenery_art/shaft_flame_frame_9_polygons.inc"
};
static Sint32 shaft_flame_frame_9_vertices[195] = {
#include "stg34/scenery_art/shaft_flame_frame_9_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_9_model = {
#include "stg34/scenery_art/shaft_flame_frame_9_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_9_object = {
#include "stg34/scenery_art/shaft_flame_frame_9_object.inc"
};
static Sint16 shaft_flame_frame_8_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_8_polygons.inc"
};
static Sint32 shaft_flame_frame_8_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_8_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_8_model = {
#include "stg34/scenery_art/shaft_flame_frame_8_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_8_object = {
#include "stg34/scenery_art/shaft_flame_frame_8_object.inc"
};
static Sint16 shaft_flame_frame_7_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_7_polygons.inc"
};
static Sint32 shaft_flame_frame_7_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_7_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_7_model = {
#include "stg34/scenery_art/shaft_flame_frame_7_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_7_object = {
#include "stg34/scenery_art/shaft_flame_frame_7_object.inc"
};
static Sint16 shaft_flame_frame_6_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_6_polygons.inc"
};
static Sint32 shaft_flame_frame_6_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_6_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_6_model = {
#include "stg34/scenery_art/shaft_flame_frame_6_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_6_object = {
#include "stg34/scenery_art/shaft_flame_frame_6_object.inc"
};
static Sint16 shaft_flame_frame_5_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_5_polygons.inc"
};
static Sint32 shaft_flame_frame_5_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_5_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_5_model = {
#include "stg34/scenery_art/shaft_flame_frame_5_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_5_object = {
#include "stg34/scenery_art/shaft_flame_frame_5_object.inc"
};
static Sint16 shaft_flame_frame_4_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_4_polygons.inc"
};
static Sint32 shaft_flame_frame_4_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_4_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_4_model = {
#include "stg34/scenery_art/shaft_flame_frame_4_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_4_object = {
#include "stg34/scenery_art/shaft_flame_frame_4_object.inc"
};
static Sint16 shaft_flame_frame_3_polygons[325] = {
#include "stg34/scenery_art/shaft_flame_frame_3_polygons.inc"
};
static Sint32 shaft_flame_frame_3_vertices[387] = {
#include "stg34/scenery_art/shaft_flame_frame_3_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_3_model = {
#include "stg34/scenery_art/shaft_flame_frame_3_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_3_object = {
#include "stg34/scenery_art/shaft_flame_frame_3_object.inc"
};
static Sint16 shaft_flame_frame_2_polygons[321] = {
#include "stg34/scenery_art/shaft_flame_frame_2_polygons.inc"
};
static Sint32 shaft_flame_frame_2_vertices[383] = {
#include "stg34/scenery_art/shaft_flame_frame_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_2_model = {
#include "stg34/scenery_art/shaft_flame_frame_2_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_2_object = {
#include "stg34/scenery_art/shaft_flame_frame_2_object.inc"
};
static Sint16 shaft_flame_frame_1_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_1_polygons.inc"
};
static Sint32 shaft_flame_frame_1_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_1_model = {
#include "stg34/scenery_art/shaft_flame_frame_1_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_1_object = {
#include "stg34/scenery_art/shaft_flame_frame_1_object.inc"
};
static Sint16 shaft_flame_frame_0_polygons[313] = {
#include "stg34/scenery_art/shaft_flame_frame_0_polygons.inc"
};
static Sint32 shaft_flame_frame_0_vertices[371] = {
#include "stg34/scenery_art/shaft_flame_frame_0_vertices.inc"
};
static NJS_CNK_MODEL shaft_flame_frame_0_model = {
#include "stg34/scenery_art/shaft_flame_frame_0_model.inc"
};
static NJS_CNK_OBJECT shaft_flame_frame_0_object = {
#include "stg34/scenery_art/shaft_flame_frame_0_object.inc"
};
static NJS_CNK_OBJECT shaft_flame_sequence_part_0 = {
#include "stg34/scenery_art/shaft_flame_sequence_part_0.inc"
};
static NJS_TEXNAME shaft_unit_textures_names[1] = {
#include "stg34/scenery_art/shaft_unit_textures_names.inc"
};
NJS_TEXLIST shaft_unit_textures = {
#include "stg34/scenery_art/shaft_unit_textures.inc"
};
static Sint16 shaft_unit_frame_15_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_15_polygons.inc"
};
static Sint32 shaft_unit_frame_15_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_15_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_15_model = {
#include "stg34/scenery_art/shaft_unit_frame_15_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_15_object = {
#include "stg34/scenery_art/shaft_unit_frame_15_object.inc"
};
static Sint16 shaft_unit_frame_14_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_14_polygons.inc"
};
static Sint32 shaft_unit_frame_14_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_14_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_14_model = {
#include "stg34/scenery_art/shaft_unit_frame_14_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_14_object = {
#include "stg34/scenery_art/shaft_unit_frame_14_object.inc"
};
static Sint16 shaft_unit_frame_13_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_13_polygons.inc"
};
static Sint32 shaft_unit_frame_13_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_13_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_13_model = {
#include "stg34/scenery_art/shaft_unit_frame_13_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_13_object = {
#include "stg34/scenery_art/shaft_unit_frame_13_object.inc"
};
static Sint16 shaft_unit_frame_12_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_12_polygons.inc"
};
static Sint32 shaft_unit_frame_12_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_12_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_12_model = {
#include "stg34/scenery_art/shaft_unit_frame_12_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_12_object = {
#include "stg34/scenery_art/shaft_unit_frame_12_object.inc"
};
static Sint16 shaft_unit_frame_11_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_11_polygons.inc"
};
static Sint32 shaft_unit_frame_11_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_11_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_11_model = {
#include "stg34/scenery_art/shaft_unit_frame_11_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_11_object = {
#include "stg34/scenery_art/shaft_unit_frame_11_object.inc"
};
static Sint16 shaft_unit_frame_10_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_10_polygons.inc"
};
static Sint32 shaft_unit_frame_10_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_10_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_10_model = {
#include "stg34/scenery_art/shaft_unit_frame_10_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_10_object = {
#include "stg34/scenery_art/shaft_unit_frame_10_object.inc"
};
static Sint16 shaft_unit_frame_9_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_9_polygons.inc"
};
static Sint32 shaft_unit_frame_9_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_9_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_9_model = {
#include "stg34/scenery_art/shaft_unit_frame_9_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_9_object = {
#include "stg34/scenery_art/shaft_unit_frame_9_object.inc"
};
static Sint16 shaft_unit_frame_8_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_8_polygons.inc"
};
static Sint32 shaft_unit_frame_8_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_8_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_8_model = {
#include "stg34/scenery_art/shaft_unit_frame_8_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_8_object = {
#include "stg34/scenery_art/shaft_unit_frame_8_object.inc"
};
static Sint16 shaft_unit_frame_7_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_7_polygons.inc"
};
static Sint32 shaft_unit_frame_7_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_7_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_7_model = {
#include "stg34/scenery_art/shaft_unit_frame_7_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_7_object = {
#include "stg34/scenery_art/shaft_unit_frame_7_object.inc"
};
static Sint16 shaft_unit_frame_6_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_6_polygons.inc"
};
static Sint32 shaft_unit_frame_6_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_6_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_6_model = {
#include "stg34/scenery_art/shaft_unit_frame_6_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_6_object = {
#include "stg34/scenery_art/shaft_unit_frame_6_object.inc"
};
static Sint16 shaft_unit_frame_5_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_5_polygons.inc"
};
static Sint32 shaft_unit_frame_5_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_5_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_5_model = {
#include "stg34/scenery_art/shaft_unit_frame_5_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_5_object = {
#include "stg34/scenery_art/shaft_unit_frame_5_object.inc"
};
static Sint16 shaft_unit_frame_4_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_4_polygons.inc"
};
static Sint32 shaft_unit_frame_4_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_4_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_4_model = {
#include "stg34/scenery_art/shaft_unit_frame_4_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_4_object = {
#include "stg34/scenery_art/shaft_unit_frame_4_object.inc"
};
static Sint16 shaft_unit_frame_3_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_3_polygons.inc"
};
static Sint32 shaft_unit_frame_3_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_3_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_3_model = {
#include "stg34/scenery_art/shaft_unit_frame_3_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_3_object = {
#include "stg34/scenery_art/shaft_unit_frame_3_object.inc"
};
static Sint16 shaft_unit_frame_2_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_2_polygons.inc"
};
static Sint32 shaft_unit_frame_2_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_2_model = {
#include "stg34/scenery_art/shaft_unit_frame_2_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_2_object = {
#include "stg34/scenery_art/shaft_unit_frame_2_object.inc"
};
static Sint16 shaft_unit_frame_1_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_1_polygons.inc"
};
static Sint32 shaft_unit_frame_1_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_1_model = {
#include "stg34/scenery_art/shaft_unit_frame_1_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_1_object = {
#include "stg34/scenery_art/shaft_unit_frame_1_object.inc"
};
static Sint16 shaft_unit_frame_0_polygons[427] = {
#include "stg34/scenery_art/shaft_unit_frame_0_polygons.inc"
};
static Sint32 shaft_unit_frame_0_vertices[315] = {
#include "stg34/scenery_art/shaft_unit_frame_0_vertices.inc"
};
static NJS_CNK_MODEL shaft_unit_frame_0_model = {
#include "stg34/scenery_art/shaft_unit_frame_0_model.inc"
};
static NJS_CNK_OBJECT shaft_unit_frame_0_object = {
#include "stg34/scenery_art/shaft_unit_frame_0_object.inc"
};
static NJS_CNK_OBJECT shaft_unit_sequence_part_0 = {
#include "stg34/scenery_art/shaft_unit_sequence_part_0.inc"
};
static NJS_TEXNAME shaft_motion_textures_names[1] = {
#include "stg34/scenery_art/shaft_motion_textures_names.inc"
};
NJS_TEXLIST shaft_motion_textures = {
#include "stg34/scenery_art/shaft_motion_textures.inc"
};
static Sint16 shaft_assembly_part_16_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_16_polygons.inc"
};
static Sint32 shaft_assembly_part_16_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_16_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_16_model = {
#include "stg34/scenery_art/shaft_assembly_part_16_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_16 = {
#include "stg34/scenery_art/shaft_assembly_part_16.inc"
};
static Sint16 shaft_assembly_part_15_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_15_polygons.inc"
};
static Sint32 shaft_assembly_part_15_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_15_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_15_model = {
#include "stg34/scenery_art/shaft_assembly_part_15_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_15 = {
#include "stg34/scenery_art/shaft_assembly_part_15.inc"
};
static Sint16 shaft_assembly_part_14_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_14_polygons.inc"
};
static Sint32 shaft_assembly_part_14_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_14_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_14_model = {
#include "stg34/scenery_art/shaft_assembly_part_14_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_14 = {
#include "stg34/scenery_art/shaft_assembly_part_14.inc"
};
static Sint16 shaft_assembly_part_13_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_13_polygons.inc"
};
static Sint32 shaft_assembly_part_13_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_13_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_13_model = {
#include "stg34/scenery_art/shaft_assembly_part_13_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_13 = {
#include "stg34/scenery_art/shaft_assembly_part_13.inc"
};
static Sint16 shaft_assembly_part_12_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_12_polygons.inc"
};
static Sint32 shaft_assembly_part_12_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_12_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_12_model = {
#include "stg34/scenery_art/shaft_assembly_part_12_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_12 = {
#include "stg34/scenery_art/shaft_assembly_part_12.inc"
};
static Sint16 shaft_assembly_part_11_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_11_polygons.inc"
};
static Sint32 shaft_assembly_part_11_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_11_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_11_model = {
#include "stg34/scenery_art/shaft_assembly_part_11_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_11 = {
#include "stg34/scenery_art/shaft_assembly_part_11.inc"
};
static Sint16 shaft_assembly_part_10_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_10_polygons.inc"
};
static Sint32 shaft_assembly_part_10_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_10_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_10_model = {
#include "stg34/scenery_art/shaft_assembly_part_10_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_10 = {
#include "stg34/scenery_art/shaft_assembly_part_10.inc"
};
static Sint16 shaft_assembly_part_9_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_9_polygons.inc"
};
static Sint32 shaft_assembly_part_9_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_9_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_9_model = {
#include "stg34/scenery_art/shaft_assembly_part_9_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_9 = {
#include "stg34/scenery_art/shaft_assembly_part_9.inc"
};
static Sint16 shaft_assembly_part_8_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_8_polygons.inc"
};
static Sint32 shaft_assembly_part_8_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_8_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_8_model = {
#include "stg34/scenery_art/shaft_assembly_part_8_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_8 = {
#include "stg34/scenery_art/shaft_assembly_part_8.inc"
};
static Sint16 shaft_assembly_part_7_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_7_polygons.inc"
};
static Sint32 shaft_assembly_part_7_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_7_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_7_model = {
#include "stg34/scenery_art/shaft_assembly_part_7_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_7 = {
#include "stg34/scenery_art/shaft_assembly_part_7.inc"
};
static Sint16 shaft_assembly_part_6_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_6_polygons.inc"
};
static Sint32 shaft_assembly_part_6_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_6_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_6_model = {
#include "stg34/scenery_art/shaft_assembly_part_6_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_6 = {
#include "stg34/scenery_art/shaft_assembly_part_6.inc"
};
static Sint16 shaft_assembly_part_5_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_5_polygons.inc"
};
static Sint32 shaft_assembly_part_5_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_5_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_5_model = {
#include "stg34/scenery_art/shaft_assembly_part_5_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_5 = {
#include "stg34/scenery_art/shaft_assembly_part_5.inc"
};
static Sint16 shaft_assembly_part_4_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_4_polygons.inc"
};
static Sint32 shaft_assembly_part_4_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_4_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_4_model = {
#include "stg34/scenery_art/shaft_assembly_part_4_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_4 = {
#include "stg34/scenery_art/shaft_assembly_part_4.inc"
};
static Sint16 shaft_assembly_part_3_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_3_polygons.inc"
};
static Sint32 shaft_assembly_part_3_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_3_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_3_model = {
#include "stg34/scenery_art/shaft_assembly_part_3_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_3 = {
#include "stg34/scenery_art/shaft_assembly_part_3.inc"
};
static Sint16 shaft_assembly_part_2_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_2_polygons.inc"
};
static Sint32 shaft_assembly_part_2_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_2_model = {
#include "stg34/scenery_art/shaft_assembly_part_2_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_2 = {
#include "stg34/scenery_art/shaft_assembly_part_2.inc"
};
static Sint16 shaft_assembly_part_1_polygons[285] = {
#include "stg34/scenery_art/shaft_assembly_part_1_polygons.inc"
};
static Sint32 shaft_assembly_part_1_vertices[99] = {
#include "stg34/scenery_art/shaft_assembly_part_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_assembly_part_1_model = {
#include "stg34/scenery_art/shaft_assembly_part_1_model.inc"
};
static NJS_CNK_OBJECT shaft_assembly_part_1 = {
#include "stg34/scenery_art/shaft_assembly_part_1.inc"
};
NJS_CNK_OBJECT shaft_assembly = {
#include "stg34/scenery_art/shaft_assembly.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_0_position[2] = {
#include "stg34/scenery_art/core_shaft_motion_joint_0_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_0_angles[2] = {
#include "stg34/scenery_art/core_shaft_motion_joint_0_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_1_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_1_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_1_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_1_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_2_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_2_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_2_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_2_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_3_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_3_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_3_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_3_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_4_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_4_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_4_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_4_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_5_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_5_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_5_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_5_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_6_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_6_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_6_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_6_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_7_position[184] = {
#include "stg34/scenery_art/core_shaft_motion_joint_7_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_7_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_7_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_8_position[185] = {
#include "stg34/scenery_art/core_shaft_motion_joint_8_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_8_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_8_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_9_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_9_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_9_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_9_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_10_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_10_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_10_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_10_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_11_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_11_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_11_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_11_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_12_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_12_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_12_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_12_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_13_position[184] = {
#include "stg34/scenery_art/core_shaft_motion_joint_13_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_13_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_13_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_14_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_14_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_14_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_14_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_15_position[183] = {
#include "stg34/scenery_art/core_shaft_motion_joint_15_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_15_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_15_angles.inc"
};
static NJS_MKEY_F core_shaft_motion_joint_16_position[182] = {
#include "stg34/scenery_art/core_shaft_motion_joint_16_position.inc"
};
static NJS_MKEY_A core_shaft_motion_joint_16_angles[191] = {
#include "stg34/scenery_art/core_shaft_motion_joint_16_angles.inc"
};
static NJS_MDATA3 core_shaft_motion_tracks[17] = {
#include "stg34/scenery_art/core_shaft_motion_tracks.inc"
};
NJS_MOTION core_shaft_motion = {
#include "stg34/scenery_art/core_shaft_motion.inc"
};
static NJS_TEXNAME shutter_textures_names[1] = {
#include "stg34/scenery_art/shutter_textures_names.inc"
};
NJS_TEXLIST shutter_textures = {
#include "stg34/scenery_art/shutter_textures.inc"
};
static Sint16 shutter_odd_polygons[89] = {
#include "stg34/scenery_art/shutter_odd_polygons.inc"
};
static Sint32 shutter_odd_vertices[99] = {
#include "stg34/scenery_art/shutter_odd_vertices.inc"
};
static NJS_CNK_MODEL shutter_odd_model = {
#include "stg34/scenery_art/shutter_odd_model.inc"
};
NJS_CNK_OBJECT shutter_odd = {
#include "stg34/scenery_art/shutter_odd.inc"
};
static Sint16 shutter_even_polygons[139] = {
#include "stg34/scenery_art/shutter_even_polygons.inc"
};
static Sint32 shutter_even_vertices[163] = {
#include "stg34/scenery_art/shutter_even_vertices.inc"
};
static NJS_CNK_MODEL shutter_even_model = {
#include "stg34/scenery_art/shutter_even_model.inc"
};
NJS_CNK_OBJECT shutter_even = {
#include "stg34/scenery_art/shutter_even.inc"
};
static NJS_CNK_OBJECT shutter_even_sequence_part_0 = {
#include "stg34/scenery_art/shutter_even_sequence_part_0.inc"
};
static Sint16 fan_walls_part_1_polygons[105] = {
#include "stg34/scenery_art/fan_walls_part_1_polygons.inc"
};
static Sint32 fan_walls_part_1_vertices[307] = {
#include "stg34/scenery_art/fan_walls_part_1_vertices.inc"
};
static NJS_CNK_MODEL fan_walls_part_1_model = {
#include "stg34/scenery_art/fan_walls_part_1_model.inc"
};
static NJS_CNK_OBJECT fan_walls_part_1 = {
#include "stg34/scenery_art/fan_walls_part_1.inc"
};
NJS_CNK_OBJECT fan_walls = {
#include "stg34/scenery_art/fan_walls.inc"
};
static NJS_TEXNAME fan_textures_names[3] = {
#include "stg34/scenery_art/fan_textures_names.inc"
};
NJS_TEXLIST fan_textures = {
#include "stg34/scenery_art/fan_textures.inc"
};
static Sint16 core_rotor_array_part_3_polygons[621] = {
#include "stg34/scenery_art/core_rotor_array_part_3_polygons.inc"
};
static Sint32 core_rotor_array_part_3_vertices[587] = {
#include "stg34/scenery_art/core_rotor_array_part_3_vertices.inc"
};
static NJS_CNK_MODEL core_rotor_array_part_3_model = {
#include "stg34/scenery_art/core_rotor_array_part_3_model.inc"
};
static NJS_CNK_OBJECT core_rotor_array_part_3 = {
#include "stg34/scenery_art/core_rotor_array_part_3.inc"
};
static Sint16 core_rotor_array_part_2_polygons[621] = {
#include "stg34/scenery_art/core_rotor_array_part_2_polygons.inc"
};
static Sint32 core_rotor_array_part_2_vertices[587] = {
#include "stg34/scenery_art/core_rotor_array_part_2_vertices.inc"
};
NJS_CNK_MODEL reverse_fan[1] = {
#include "stg34/scenery_art/reverse_fan.inc"
};
static NJS_CNK_OBJECT core_rotor_array_part_2 = {
#include "stg34/scenery_art/core_rotor_array_part_2.inc"
};
static Sint16 core_rotor_array_part_1_polygons[621] = {
#include "stg34/scenery_art/core_rotor_array_part_1_polygons.inc"
};
static Sint32 core_rotor_array_part_1_vertices[587] = {
#include "stg34/scenery_art/core_rotor_array_part_1_vertices.inc"
};
static NJS_CNK_MODEL core_rotor_array_part_1_model = {
#include "stg34/scenery_art/core_rotor_array_part_1_model.inc"
};
static NJS_CNK_OBJECT core_rotor_array_part_1 = {
#include "stg34/scenery_art/core_rotor_array_part_1.inc"
};
NJS_CNK_OBJECT core_rotor_array[1] = {
#include "stg34/scenery_art/core_rotor_array.inc"
};
static NJS_TEXNAME bobbing_water_archive_textures_names[1] = {
#include "stg34/scenery_art/bobbing_water_archive_textures_names.inc"
};
static NJS_TEXLIST bobbing_water_archive_textures = {
#include "stg34/scenery_art/bobbing_water_archive_textures.inc"
};
static Sint16 bobbing_water_part_1_polygons[131] = {
#include "stg34/scenery_art/bobbing_water_part_1_polygons.inc"
};
static Sint32 bobbing_water_part_1_vertices[99] = {
#include "stg34/scenery_art/bobbing_water_part_1_vertices.inc"
};
NJS_CNK_MODEL bobbing_water_model[1] = {
#include "stg34/scenery_art/bobbing_water_model.inc"
};
static NJS_CNK_OBJECT bobbing_water_part_1 = {
#include "stg34/scenery_art/bobbing_water_part_1.inc"
};
NJS_CNK_OBJECT bobbing_water = {
#include "stg34/scenery_art/bobbing_water.inc"
};
static Sint16 rotor_water_uvs[88] = {
#include "stg34/scenery_art/rotor_water_uvs.inc"
};
static struct texture_anim rotor_water_scroll = {
#include "stg34/scenery_art/rotor_water_scroll.inc"
};
static NJS_MATERIAL rotor_collision_material[1] = {
#include "stg34/scenery_art/rotor_collision_material.inc"
};
static Sint16 rotor_collision_indices[129] = {
#include "stg34/scenery_art/rotor_collision_indices.inc"
};
static NJS_MESHSET rotor_collision_meshset[1] = {
#include "stg34/scenery_art/rotor_collision_meshset.inc"
};
static NJS_POINT3 rotor_collision_points[48] = {
#include "stg34/scenery_art/rotor_collision_points.inc"
};
static NJS_POINT3 rotor_collision_normals[48] = {
#include "stg34/scenery_art/rotor_collision_normals.inc"
};
static NJS_MODEL rotor_collision_basic = {
#include "stg34/scenery_art/rotor_collision_basic.inc"
};
static NJS_OBJECT rotor_collision_solid_node = {
#include "stg34/scenery_art/rotor_collision_solid_node.inc"
};
static NJS_TEXNAME fan_wall_textures_names[1] = {
#include "stg34/scenery_art/fan_wall_textures_names.inc"
};
NJS_TEXLIST fan_wall_textures = {
#include "stg34/scenery_art/fan_wall_textures.inc"
};
static Sint16 fan_wall_frame_7_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_7_polygons.inc"
};
static Sint32 fan_wall_frame_7_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_7_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_7_model = {
#include "stg34/scenery_art/fan_wall_frame_7_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_7_object = {
#include "stg34/scenery_art/fan_wall_frame_7_object.inc"
};
static Sint16 fan_wall_frame_6_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_6_polygons.inc"
};
static Sint32 fan_wall_frame_6_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_6_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_6_model = {
#include "stg34/scenery_art/fan_wall_frame_6_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_6_object = {
#include "stg34/scenery_art/fan_wall_frame_6_object.inc"
};
static Sint16 fan_wall_frame_5_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_5_polygons.inc"
};
static Sint32 fan_wall_frame_5_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_5_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_5_model = {
#include "stg34/scenery_art/fan_wall_frame_5_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_5_object = {
#include "stg34/scenery_art/fan_wall_frame_5_object.inc"
};
static Sint16 fan_wall_frame_4_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_4_polygons.inc"
};
static Sint32 fan_wall_frame_4_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_4_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_4_model = {
#include "stg34/scenery_art/fan_wall_frame_4_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_4_object = {
#include "stg34/scenery_art/fan_wall_frame_4_object.inc"
};
static Sint16 fan_wall_frame_3_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_3_polygons.inc"
};
static Sint32 fan_wall_frame_3_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_3_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_3_model = {
#include "stg34/scenery_art/fan_wall_frame_3_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_3_object = {
#include "stg34/scenery_art/fan_wall_frame_3_object.inc"
};
static Sint16 fan_wall_frame_2_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_2_polygons.inc"
};
static Sint32 fan_wall_frame_2_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_2_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_2_model = {
#include "stg34/scenery_art/fan_wall_frame_2_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_2_object = {
#include "stg34/scenery_art/fan_wall_frame_2_object.inc"
};
static Sint16 fan_wall_frame_1_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_1_polygons.inc"
};
static Sint32 fan_wall_frame_1_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_1_model = {
#include "stg34/scenery_art/fan_wall_frame_1_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_1_object = {
#include "stg34/scenery_art/fan_wall_frame_1_object.inc"
};
static Sint16 fan_wall_frame_0_polygons[315] = {
#include "stg34/scenery_art/fan_wall_frame_0_polygons.inc"
};
static Sint32 fan_wall_frame_0_vertices[307] = {
#include "stg34/scenery_art/fan_wall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL fan_wall_frame_0_model = {
#include "stg34/scenery_art/fan_wall_frame_0_model.inc"
};
static NJS_CNK_OBJECT fan_wall_frame_0_object = {
#include "stg34/scenery_art/fan_wall_frame_0_object.inc"
};
static NJS_CNK_OBJECT fan_wall_sequence_part_0 = {
#include "stg34/scenery_art/fan_wall_sequence_part_0.inc"
};
static Sint16 core_pillar_upper_part_14_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_14_polygons.inc"
};
static Sint32 core_pillar_upper_part_14_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_14_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_14_model = {
#include "stg34/scenery_art/core_pillar_upper_part_14_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_14 = {
#include "stg34/scenery_art/core_pillar_upper_part_14.inc"
};
static Sint16 core_pillar_upper_part_13_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_13_polygons.inc"
};
static Sint32 core_pillar_upper_part_13_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_13_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_13_model = {
#include "stg34/scenery_art/core_pillar_upper_part_13_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_13 = {
#include "stg34/scenery_art/core_pillar_upper_part_13.inc"
};
static Sint16 core_pillar_upper_part_12_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_12_polygons.inc"
};
static Sint32 core_pillar_upper_part_12_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_12_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_12_model = {
#include "stg34/scenery_art/core_pillar_upper_part_12_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_12 = {
#include "stg34/scenery_art/core_pillar_upper_part_12.inc"
};
static Sint16 core_pillar_upper_part_11_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_11_polygons.inc"
};
static Sint32 core_pillar_upper_part_11_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_11_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_11_model = {
#include "stg34/scenery_art/core_pillar_upper_part_11_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_11 = {
#include "stg34/scenery_art/core_pillar_upper_part_11.inc"
};
static Sint16 core_pillar_upper_part_10_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_10_polygons.inc"
};
static Sint32 core_pillar_upper_part_10_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_10_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_10_model = {
#include "stg34/scenery_art/core_pillar_upper_part_10_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_10 = {
#include "stg34/scenery_art/core_pillar_upper_part_10.inc"
};
static Sint16 core_pillar_upper_part_9_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_9_polygons.inc"
};
static Sint32 core_pillar_upper_part_9_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_9_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_9_model = {
#include "stg34/scenery_art/core_pillar_upper_part_9_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_9 = {
#include "stg34/scenery_art/core_pillar_upper_part_9.inc"
};
static Sint16 core_pillar_upper_part_8_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_8_polygons.inc"
};
static Sint32 core_pillar_upper_part_8_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_8_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_8_model = {
#include "stg34/scenery_art/core_pillar_upper_part_8_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_8 = {
#include "stg34/scenery_art/core_pillar_upper_part_8.inc"
};
static Sint16 core_pillar_upper_part_7_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_7_polygons.inc"
};
static Sint32 core_pillar_upper_part_7_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_7_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_7_model = {
#include "stg34/scenery_art/core_pillar_upper_part_7_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_7 = {
#include "stg34/scenery_art/core_pillar_upper_part_7.inc"
};
static Sint16 core_pillar_upper_part_6_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_6_polygons.inc"
};
static Sint32 core_pillar_upper_part_6_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_6_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_6_model = {
#include "stg34/scenery_art/core_pillar_upper_part_6_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_6 = {
#include "stg34/scenery_art/core_pillar_upper_part_6.inc"
};
static Sint16 core_pillar_upper_part_5_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_5_polygons.inc"
};
static Sint32 core_pillar_upper_part_5_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_5_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_5_model = {
#include "stg34/scenery_art/core_pillar_upper_part_5_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_5 = {
#include "stg34/scenery_art/core_pillar_upper_part_5.inc"
};
static Sint16 core_pillar_upper_part_4_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_4_polygons.inc"
};
static Sint32 core_pillar_upper_part_4_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_4_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_4_model = {
#include "stg34/scenery_art/core_pillar_upper_part_4_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_4 = {
#include "stg34/scenery_art/core_pillar_upper_part_4.inc"
};
static Sint16 core_pillar_upper_part_3_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_3_polygons.inc"
};
static Sint32 core_pillar_upper_part_3_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_3_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_3_model = {
#include "stg34/scenery_art/core_pillar_upper_part_3_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_3 = {
#include "stg34/scenery_art/core_pillar_upper_part_3.inc"
};
static Sint16 core_pillar_upper_part_2_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_2_polygons.inc"
};
static Sint32 core_pillar_upper_part_2_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_2_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_2_model = {
#include "stg34/scenery_art/core_pillar_upper_part_2_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_2 = {
#include "stg34/scenery_art/core_pillar_upper_part_2.inc"
};
static Sint16 core_pillar_upper_part_1_polygons[263] = {
#include "stg34/scenery_art/core_pillar_upper_part_1_polygons.inc"
};
static Sint32 core_pillar_upper_part_1_vertices[483] = {
#include "stg34/scenery_art/core_pillar_upper_part_1_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_upper_part_1_model = {
#include "stg34/scenery_art/core_pillar_upper_part_1_model.inc"
};
static NJS_CNK_OBJECT core_pillar_upper_part_1 = {
#include "stg34/scenery_art/core_pillar_upper_part_1.inc"
};
NJS_CNK_OBJECT core_pillar_upper = {
#include "stg34/scenery_art/core_pillar_upper.inc"
};
static Sint16 core_pillar_lower_part_6_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_6_polygons.inc"
};
static Sint32 core_pillar_lower_part_6_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_6_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_6_model = {
#include "stg34/scenery_art/core_pillar_lower_part_6_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_6 = {
#include "stg34/scenery_art/core_pillar_lower_part_6.inc"
};
static Sint16 core_pillar_lower_part_5_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_5_polygons.inc"
};
static Sint32 core_pillar_lower_part_5_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_5_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_5_model = {
#include "stg34/scenery_art/core_pillar_lower_part_5_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_5 = {
#include "stg34/scenery_art/core_pillar_lower_part_5.inc"
};
static Sint16 core_pillar_lower_part_4_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_4_polygons.inc"
};
static Sint32 core_pillar_lower_part_4_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_4_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_4_model = {
#include "stg34/scenery_art/core_pillar_lower_part_4_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_4 = {
#include "stg34/scenery_art/core_pillar_lower_part_4.inc"
};
static Sint16 core_pillar_lower_part_3_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_3_polygons.inc"
};
static Sint32 core_pillar_lower_part_3_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_3_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_3_model = {
#include "stg34/scenery_art/core_pillar_lower_part_3_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_3 = {
#include "stg34/scenery_art/core_pillar_lower_part_3.inc"
};
static Sint16 core_pillar_lower_part_2_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_2_polygons.inc"
};
static Sint32 core_pillar_lower_part_2_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_2_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_2_model = {
#include "stg34/scenery_art/core_pillar_lower_part_2_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_2 = {
#include "stg34/scenery_art/core_pillar_lower_part_2.inc"
};
static Sint16 core_pillar_lower_part_1_polygons[263] = {
#include "stg34/scenery_art/core_pillar_lower_part_1_polygons.inc"
};
static Sint32 core_pillar_lower_part_1_vertices[483] = {
#include "stg34/scenery_art/core_pillar_lower_part_1_vertices.inc"
};
static NJS_CNK_MODEL core_pillar_lower_part_1_model = {
#include "stg34/scenery_art/core_pillar_lower_part_1_model.inc"
};
static NJS_CNK_OBJECT core_pillar_lower_part_1 = {
#include "stg34/scenery_art/core_pillar_lower_part_1.inc"
};
NJS_CNK_OBJECT core_pillar_lower = {
#include "stg34/scenery_art/core_pillar_lower.inc"
};
static Sint16 water_units_part_2_polygons[65] = {
#include "stg34/scenery_art/water_units_part_2_polygons.inc"
};
static Sint32 water_units_part_2_vertices[107] = {
#include "stg34/scenery_art/water_units_part_2_vertices.inc"
};
static NJS_CNK_MODEL water_units_part_2_model = {
#include "stg34/scenery_art/water_units_part_2_model.inc"
};
static NJS_CNK_OBJECT water_units_part_2 = {
#include "stg34/scenery_art/water_units_part_2.inc"
};
static Sint16 water_units_part_1_polygons[65] = {
#include "stg34/scenery_art/water_units_part_1_polygons.inc"
};
static Sint32 water_units_part_1_vertices[107] = {
#include "stg34/scenery_art/water_units_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_units_part_1_model = {
#include "stg34/scenery_art/water_units_part_1_model.inc"
};
static NJS_CNK_OBJECT water_units_part_1 = {
#include "stg34/scenery_art/water_units_part_1.inc"
};
NJS_CNK_OBJECT water_units = {
#include "stg34/scenery_art/water_units.inc"
};
static Sint16 upper_channel_entry_part_2_polygons[65] = {
#include "stg34/scenery_art/upper_channel_entry_part_2_polygons.inc"
};
static Sint32 upper_channel_entry_part_2_vertices[107] = {
#include "stg34/scenery_art/upper_channel_entry_part_2_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_entry_part_2_model = {
#include "stg34/scenery_art/upper_channel_entry_part_2_model.inc"
};
static NJS_CNK_OBJECT upper_channel_entry_part_2 = {
#include "stg34/scenery_art/upper_channel_entry_part_2.inc"
};
static Sint16 upper_channel_entry_part_1_polygons[65] = {
#include "stg34/scenery_art/upper_channel_entry_part_1_polygons.inc"
};
static Sint32 upper_channel_entry_part_1_vertices[107] = {
#include "stg34/scenery_art/upper_channel_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_entry_part_1_model = {
#include "stg34/scenery_art/upper_channel_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT upper_channel_entry_part_1 = {
#include "stg34/scenery_art/upper_channel_entry_part_1.inc"
};
NJS_CNK_OBJECT upper_channel_entry = {
#include "stg34/scenery_art/upper_channel_entry.inc"
};
static Sint16 upper_channel_exit_part_2_polygons[65] = {
#include "stg34/scenery_art/upper_channel_exit_part_2_polygons.inc"
};
static Sint32 upper_channel_exit_part_2_vertices[107] = {
#include "stg34/scenery_art/upper_channel_exit_part_2_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_exit_part_2_model = {
#include "stg34/scenery_art/upper_channel_exit_part_2_model.inc"
};
static NJS_CNK_OBJECT upper_channel_exit_part_2 = {
#include "stg34/scenery_art/upper_channel_exit_part_2.inc"
};
static Sint16 upper_channel_exit_part_1_polygons[65] = {
#include "stg34/scenery_art/upper_channel_exit_part_1_polygons.inc"
};
static Sint32 upper_channel_exit_part_1_vertices[107] = {
#include "stg34/scenery_art/upper_channel_exit_part_1_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_exit_part_1_model = {
#include "stg34/scenery_art/upper_channel_exit_part_1_model.inc"
};
static NJS_CNK_OBJECT upper_channel_exit_part_1 = {
#include "stg34/scenery_art/upper_channel_exit_part_1.inc"
};
NJS_CNK_OBJECT upper_channel_exit = {
#include "stg34/scenery_art/upper_channel_exit.inc"
};
static Sint16 middle_channel_entry_part_2_polygons[65] = {
#include "stg34/scenery_art/middle_channel_entry_part_2_polygons.inc"
};
static Sint32 middle_channel_entry_part_2_vertices[107] = {
#include "stg34/scenery_art/middle_channel_entry_part_2_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_entry_part_2_model = {
#include "stg34/scenery_art/middle_channel_entry_part_2_model.inc"
};
static NJS_CNK_OBJECT middle_channel_entry_part_2 = {
#include "stg34/scenery_art/middle_channel_entry_part_2.inc"
};
static Sint16 middle_channel_entry_part_1_polygons[65] = {
#include "stg34/scenery_art/middle_channel_entry_part_1_polygons.inc"
};
static Sint32 middle_channel_entry_part_1_vertices[107] = {
#include "stg34/scenery_art/middle_channel_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_entry_part_1_model = {
#include "stg34/scenery_art/middle_channel_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT middle_channel_entry_part_1 = {
#include "stg34/scenery_art/middle_channel_entry_part_1.inc"
};
NJS_CNK_OBJECT middle_channel_entry = {
#include "stg34/scenery_art/middle_channel_entry.inc"
};
static Sint16 middle_channel_exit_part_2_polygons[65] = {
#include "stg34/scenery_art/middle_channel_exit_part_2_polygons.inc"
};
static Sint32 middle_channel_exit_part_2_vertices[107] = {
#include "stg34/scenery_art/middle_channel_exit_part_2_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_exit_part_2_model = {
#include "stg34/scenery_art/middle_channel_exit_part_2_model.inc"
};
static NJS_CNK_OBJECT middle_channel_exit_part_2 = {
#include "stg34/scenery_art/middle_channel_exit_part_2.inc"
};
static Sint16 middle_channel_exit_part_1_polygons[65] = {
#include "stg34/scenery_art/middle_channel_exit_part_1_polygons.inc"
};
static Sint32 middle_channel_exit_part_1_vertices[107] = {
#include "stg34/scenery_art/middle_channel_exit_part_1_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_exit_part_1_model = {
#include "stg34/scenery_art/middle_channel_exit_part_1_model.inc"
};
static NJS_CNK_OBJECT middle_channel_exit_part_1 = {
#include "stg34/scenery_art/middle_channel_exit_part_1.inc"
};
NJS_CNK_OBJECT middle_channel_exit = {
#include "stg34/scenery_art/middle_channel_exit.inc"
};
static Sint16 lower_channel_entry_part_4_polygons[35] = {
#include "stg34/scenery_art/lower_channel_entry_part_4_polygons.inc"
};
static Sint32 lower_channel_entry_part_4_vertices[51] = {
#include "stg34/scenery_art/lower_channel_entry_part_4_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_entry_part_4_model = {
#include "stg34/scenery_art/lower_channel_entry_part_4_model.inc"
};
static NJS_CNK_OBJECT lower_channel_entry_part_4 = {
#include "stg34/scenery_art/lower_channel_entry_part_4.inc"
};
static Sint16 lower_channel_entry_part_3_polygons[35] = {
#include "stg34/scenery_art/lower_channel_entry_part_3_polygons.inc"
};
static Sint32 lower_channel_entry_part_3_vertices[51] = {
#include "stg34/scenery_art/lower_channel_entry_part_3_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_entry_part_3_model = {
#include "stg34/scenery_art/lower_channel_entry_part_3_model.inc"
};
static NJS_CNK_OBJECT lower_channel_entry_part_3 = {
#include "stg34/scenery_art/lower_channel_entry_part_3.inc"
};
static Sint16 lower_channel_entry_part_2_polygons[35] = {
#include "stg34/scenery_art/lower_channel_entry_part_2_polygons.inc"
};
static Sint32 lower_channel_entry_part_2_vertices[51] = {
#include "stg34/scenery_art/lower_channel_entry_part_2_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_entry_part_2_model = {
#include "stg34/scenery_art/lower_channel_entry_part_2_model.inc"
};
static NJS_CNK_OBJECT lower_channel_entry_part_2 = {
#include "stg34/scenery_art/lower_channel_entry_part_2.inc"
};
static Sint16 lower_channel_entry_part_1_polygons[35] = {
#include "stg34/scenery_art/lower_channel_entry_part_1_polygons.inc"
};
static Sint32 lower_channel_entry_part_1_vertices[51] = {
#include "stg34/scenery_art/lower_channel_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_entry_part_1_model = {
#include "stg34/scenery_art/lower_channel_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT lower_channel_entry_part_1 = {
#include "stg34/scenery_art/lower_channel_entry_part_1.inc"
};
NJS_CNK_OBJECT lower_channel_entry = {
#include "stg34/scenery_art/lower_channel_entry.inc"
};
static Sint16 lower_channel_exit_part_4_polygons[35] = {
#include "stg34/scenery_art/lower_channel_exit_part_4_polygons.inc"
};
static Sint32 lower_channel_exit_part_4_vertices[51] = {
#include "stg34/scenery_art/lower_channel_exit_part_4_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_exit_part_4_model = {
#include "stg34/scenery_art/lower_channel_exit_part_4_model.inc"
};
static NJS_CNK_OBJECT lower_channel_exit_part_4 = {
#include "stg34/scenery_art/lower_channel_exit_part_4.inc"
};
static Sint16 lower_channel_exit_part_3_polygons[35] = {
#include "stg34/scenery_art/lower_channel_exit_part_3_polygons.inc"
};
static Sint32 lower_channel_exit_part_3_vertices[51] = {
#include "stg34/scenery_art/lower_channel_exit_part_3_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_exit_part_3_model = {
#include "stg34/scenery_art/lower_channel_exit_part_3_model.inc"
};
static NJS_CNK_OBJECT lower_channel_exit_part_3 = {
#include "stg34/scenery_art/lower_channel_exit_part_3.inc"
};
static Sint16 lower_channel_exit_part_2_polygons[35] = {
#include "stg34/scenery_art/lower_channel_exit_part_2_polygons.inc"
};
static Sint32 lower_channel_exit_part_2_vertices[51] = {
#include "stg34/scenery_art/lower_channel_exit_part_2_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_exit_part_2_model = {
#include "stg34/scenery_art/lower_channel_exit_part_2_model.inc"
};
static NJS_CNK_OBJECT lower_channel_exit_part_2 = {
#include "stg34/scenery_art/lower_channel_exit_part_2.inc"
};
static Sint16 lower_channel_exit_part_1_polygons[35] = {
#include "stg34/scenery_art/lower_channel_exit_part_1_polygons.inc"
};
static Sint32 lower_channel_exit_part_1_vertices[51] = {
#include "stg34/scenery_art/lower_channel_exit_part_1_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_exit_part_1_model = {
#include "stg34/scenery_art/lower_channel_exit_part_1_model.inc"
};
static NJS_CNK_OBJECT lower_channel_exit_part_1 = {
#include "stg34/scenery_art/lower_channel_exit_part_1.inc"
};
NJS_CNK_OBJECT lower_channel_exit = {
#include "stg34/scenery_art/lower_channel_exit.inc"
};
static NJS_TEXNAME water_turn_entry_archive_textures_names[1] = {
#include "stg34/scenery_art/water_turn_entry_archive_textures_names.inc"
};
static NJS_TEXLIST water_turn_entry_archive_textures = {
#include "stg34/scenery_art/water_turn_entry_archive_textures.inc"
};
static Sint16 water_turn_entry_part_9_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_9_polygons.inc"
};
static Sint32 water_turn_entry_part_9_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_9_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_9_model = {
#include "stg34/scenery_art/water_turn_entry_part_9_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_9 = {
#include "stg34/scenery_art/water_turn_entry_part_9.inc"
};
static Sint16 water_turn_entry_part_8_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_8_polygons.inc"
};
static Sint32 water_turn_entry_part_8_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_8_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_8_model = {
#include "stg34/scenery_art/water_turn_entry_part_8_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_8 = {
#include "stg34/scenery_art/water_turn_entry_part_8.inc"
};
static Sint16 water_turn_entry_part_7_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_7_polygons.inc"
};
static Sint32 water_turn_entry_part_7_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_7_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_7_model = {
#include "stg34/scenery_art/water_turn_entry_part_7_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_7 = {
#include "stg34/scenery_art/water_turn_entry_part_7.inc"
};
static Sint16 water_turn_entry_part_6_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_6_polygons.inc"
};
static Sint32 water_turn_entry_part_6_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_6_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_6_model = {
#include "stg34/scenery_art/water_turn_entry_part_6_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_6 = {
#include "stg34/scenery_art/water_turn_entry_part_6.inc"
};
static Sint16 water_turn_entry_part_5_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_5_polygons.inc"
};
static Sint32 water_turn_entry_part_5_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_5_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_5_model = {
#include "stg34/scenery_art/water_turn_entry_part_5_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_5 = {
#include "stg34/scenery_art/water_turn_entry_part_5.inc"
};
static Sint16 water_turn_entry_part_4_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_4_polygons.inc"
};
static Sint32 water_turn_entry_part_4_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_4_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_4_model = {
#include "stg34/scenery_art/water_turn_entry_part_4_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_4 = {
#include "stg34/scenery_art/water_turn_entry_part_4.inc"
};
static Sint16 water_turn_entry_part_3_polygons[39] = {
#include "stg34/scenery_art/water_turn_entry_part_3_polygons.inc"
};
static Sint32 water_turn_entry_part_3_vertices[27] = {
#include "stg34/scenery_art/water_turn_entry_part_3_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_3_model = {
#include "stg34/scenery_art/water_turn_entry_part_3_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_3 = {
#include "stg34/scenery_art/water_turn_entry_part_3.inc"
};
static Sint16 water_turn_entry_part_2_polygons[39] = {
#include "stg34/scenery_art/water_turn_entry_part_2_polygons.inc"
};
static Sint32 water_turn_entry_part_2_vertices[35] = {
#include "stg34/scenery_art/water_turn_entry_part_2_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_2_model = {
#include "stg34/scenery_art/water_turn_entry_part_2_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_2 = {
#include "stg34/scenery_art/water_turn_entry_part_2.inc"
};
static Sint16 water_turn_entry_part_1_polygons[25] = {
#include "stg34/scenery_art/water_turn_entry_part_1_polygons.inc"
};
static Sint32 water_turn_entry_part_1_vertices[19] = {
#include "stg34/scenery_art/water_turn_entry_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_turn_entry_part_1_model = {
#include "stg34/scenery_art/water_turn_entry_part_1_model.inc"
};
static NJS_CNK_OBJECT water_turn_entry_part_1 = {
#include "stg34/scenery_art/water_turn_entry_part_1.inc"
};
NJS_CNK_OBJECT water_turn_entry = {
#include "stg34/scenery_art/water_turn_entry.inc"
};
static NJS_TEXNAME water_turn_exit_archive_textures_names[1] = {
#include "stg34/scenery_art/water_turn_exit_archive_textures_names.inc"
};
static NJS_TEXLIST water_turn_exit_archive_textures = {
#include "stg34/scenery_art/water_turn_exit_archive_textures.inc"
};
static Sint16 water_turn_exit_part_1_polygons[55] = {
#include "stg34/scenery_art/water_turn_exit_part_1_polygons.inc"
};
static Sint32 water_turn_exit_part_1_vertices[39] = {
#include "stg34/scenery_art/water_turn_exit_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_turn_exit_part_1_model = {
#include "stg34/scenery_art/water_turn_exit_part_1_model.inc"
};
static NJS_CNK_OBJECT water_turn_exit_part_1 = {
#include "stg34/scenery_art/water_turn_exit_part_1.inc"
};
NJS_CNK_OBJECT water_turn_exit = {
#include "stg34/scenery_art/water_turn_exit.inc"
};
static Sint16 waterfalls_part_2_polygons[187] = {
#include "stg34/scenery_art/waterfalls_part_2_polygons.inc"
};
static Sint32 waterfalls_part_2_vertices[323] = {
#include "stg34/scenery_art/waterfalls_part_2_vertices.inc"
};
static NJS_CNK_MODEL waterfalls_part_2_model = {
#include "stg34/scenery_art/waterfalls_part_2_model.inc"
};
static NJS_CNK_OBJECT waterfalls_part_2 = {
#include "stg34/scenery_art/waterfalls_part_2.inc"
};
static Sint16 waterfalls_part_1_polygons[187] = {
#include "stg34/scenery_art/waterfalls_part_1_polygons.inc"
};
static Sint32 waterfalls_part_1_vertices[323] = {
#include "stg34/scenery_art/waterfalls_part_1_vertices.inc"
};
static NJS_CNK_MODEL waterfalls_part_1_model = {
#include "stg34/scenery_art/waterfalls_part_1_model.inc"
};
static NJS_CNK_OBJECT waterfalls_part_1 = {
#include "stg34/scenery_art/waterfalls_part_1.inc"
};
NJS_CNK_OBJECT waterfalls = {
#include "stg34/scenery_art/waterfalls.inc"
};
static Sint16 falling_water_part_2_polygons[105] = {
#include "stg34/scenery_art/falling_water_part_2_polygons.inc"
};
static Sint32 falling_water_part_2_vertices[163] = {
#include "stg34/scenery_art/falling_water_part_2_vertices.inc"
};
static NJS_CNK_MODEL falling_water_part_2_model = {
#include "stg34/scenery_art/falling_water_part_2_model.inc"
};
static NJS_CNK_OBJECT falling_water_part_2 = {
#include "stg34/scenery_art/falling_water_part_2.inc"
};
static Sint16 falling_water_part_1_polygons[105] = {
#include "stg34/scenery_art/falling_water_part_1_polygons.inc"
};
static Sint32 falling_water_part_1_vertices[163] = {
#include "stg34/scenery_art/falling_water_part_1_vertices.inc"
};
static NJS_CNK_MODEL falling_water_part_1_model = {
#include "stg34/scenery_art/falling_water_part_1_model.inc"
};
static NJS_CNK_OBJECT falling_water_part_1 = {
#include "stg34/scenery_art/falling_water_part_1.inc"
};
NJS_CNK_OBJECT falling_water = {
#include "stg34/scenery_art/falling_water.inc"
};
static Sint16 pillar_frame_29_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_29_polygons.inc"
};
static Sint32 pillar_frame_29_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_29_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_29_model = {
#include "stg34/scenery_art/pillar_frame_29_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_29_object = {
#include "stg34/scenery_art/pillar_frame_29_object.inc"
};
static Sint16 pillar_frame_28_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_28_polygons.inc"
};
static Sint32 pillar_frame_28_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_28_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_28_model = {
#include "stg34/scenery_art/pillar_frame_28_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_28_object = {
#include "stg34/scenery_art/pillar_frame_28_object.inc"
};
static Sint16 pillar_frame_27_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_27_polygons.inc"
};
static Sint32 pillar_frame_27_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_27_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_27_model = {
#include "stg34/scenery_art/pillar_frame_27_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_27_object = {
#include "stg34/scenery_art/pillar_frame_27_object.inc"
};
static Sint16 pillar_frame_26_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_26_polygons.inc"
};
static Sint32 pillar_frame_26_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_26_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_26_model = {
#include "stg34/scenery_art/pillar_frame_26_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_26_object = {
#include "stg34/scenery_art/pillar_frame_26_object.inc"
};
static Sint16 pillar_frame_25_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_25_polygons.inc"
};
static Sint32 pillar_frame_25_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_25_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_25_model = {
#include "stg34/scenery_art/pillar_frame_25_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_25_object = {
#include "stg34/scenery_art/pillar_frame_25_object.inc"
};
static Sint16 pillar_frame_24_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_24_polygons.inc"
};
static Sint32 pillar_frame_24_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_24_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_24_model = {
#include "stg34/scenery_art/pillar_frame_24_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_24_object = {
#include "stg34/scenery_art/pillar_frame_24_object.inc"
};
static Sint16 pillar_frame_23_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_23_polygons.inc"
};
static Sint32 pillar_frame_23_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_23_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_23_model = {
#include "stg34/scenery_art/pillar_frame_23_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_23_object = {
#include "stg34/scenery_art/pillar_frame_23_object.inc"
};
static Sint16 pillar_frame_22_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_22_polygons.inc"
};
static Sint32 pillar_frame_22_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_22_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_22_model = {
#include "stg34/scenery_art/pillar_frame_22_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_22_object = {
#include "stg34/scenery_art/pillar_frame_22_object.inc"
};
static Sint16 pillar_frame_21_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_21_polygons.inc"
};
static Sint32 pillar_frame_21_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_21_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_21_model = {
#include "stg34/scenery_art/pillar_frame_21_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_21_object = {
#include "stg34/scenery_art/pillar_frame_21_object.inc"
};
static Sint16 pillar_frame_20_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_20_polygons.inc"
};
static Sint32 pillar_frame_20_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_20_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_20_model = {
#include "stg34/scenery_art/pillar_frame_20_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_20_object = {
#include "stg34/scenery_art/pillar_frame_20_object.inc"
};
static Sint16 pillar_frame_19_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_19_polygons.inc"
};
static Sint32 pillar_frame_19_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_19_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_19_model = {
#include "stg34/scenery_art/pillar_frame_19_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_19_object = {
#include "stg34/scenery_art/pillar_frame_19_object.inc"
};
static Sint16 pillar_frame_18_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_18_polygons.inc"
};
static Sint32 pillar_frame_18_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_18_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_18_model = {
#include "stg34/scenery_art/pillar_frame_18_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_18_object = {
#include "stg34/scenery_art/pillar_frame_18_object.inc"
};
static Sint16 pillar_frame_17_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_17_polygons.inc"
};
static Sint32 pillar_frame_17_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_17_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_17_model = {
#include "stg34/scenery_art/pillar_frame_17_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_17_object = {
#include "stg34/scenery_art/pillar_frame_17_object.inc"
};
static Sint16 pillar_frame_16_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_16_polygons.inc"
};
static Sint32 pillar_frame_16_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_16_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_16_model = {
#include "stg34/scenery_art/pillar_frame_16_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_16_object = {
#include "stg34/scenery_art/pillar_frame_16_object.inc"
};
static Sint16 pillar_frame_0_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_0_polygons.inc"
};
static Sint32 pillar_frame_0_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_0_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_0_model = {
#include "stg34/scenery_art/pillar_frame_0_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_0_object = {
#include "stg34/scenery_art/pillar_frame_0_object.inc"
};
static Sint16 pillar_frame_14_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_14_polygons.inc"
};
static Sint32 pillar_frame_14_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_14_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_14_model = {
#include "stg34/scenery_art/pillar_frame_14_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_14_object = {
#include "stg34/scenery_art/pillar_frame_14_object.inc"
};
static Sint16 pillar_frame_13_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_13_polygons.inc"
};
static Sint32 pillar_frame_13_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_13_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_13_model = {
#include "stg34/scenery_art/pillar_frame_13_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_13_object = {
#include "stg34/scenery_art/pillar_frame_13_object.inc"
};
static Sint16 pillar_frame_12_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_12_polygons.inc"
};
static Sint32 pillar_frame_12_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_12_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_12_model = {
#include "stg34/scenery_art/pillar_frame_12_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_12_object = {
#include "stg34/scenery_art/pillar_frame_12_object.inc"
};
static Sint16 pillar_frame_11_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_11_polygons.inc"
};
static Sint32 pillar_frame_11_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_11_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_11_model = {
#include "stg34/scenery_art/pillar_frame_11_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_11_object = {
#include "stg34/scenery_art/pillar_frame_11_object.inc"
};
static Sint16 pillar_frame_10_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_10_polygons.inc"
};
static Sint32 pillar_frame_10_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_10_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_10_model = {
#include "stg34/scenery_art/pillar_frame_10_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_10_object = {
#include "stg34/scenery_art/pillar_frame_10_object.inc"
};
static Sint16 pillar_frame_9_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_9_polygons.inc"
};
static Sint32 pillar_frame_9_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_9_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_9_model = {
#include "stg34/scenery_art/pillar_frame_9_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_9_object = {
#include "stg34/scenery_art/pillar_frame_9_object.inc"
};
static Sint16 pillar_frame_8_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_8_polygons.inc"
};
static Sint32 pillar_frame_8_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_8_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_8_model = {
#include "stg34/scenery_art/pillar_frame_8_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_8_object = {
#include "stg34/scenery_art/pillar_frame_8_object.inc"
};
static Sint16 pillar_frame_7_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_7_polygons.inc"
};
static Sint32 pillar_frame_7_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_7_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_7_model = {
#include "stg34/scenery_art/pillar_frame_7_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_7_object = {
#include "stg34/scenery_art/pillar_frame_7_object.inc"
};
static Sint16 pillar_frame_6_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_6_polygons.inc"
};
static Sint32 pillar_frame_6_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_6_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_6_model = {
#include "stg34/scenery_art/pillar_frame_6_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_6_object = {
#include "stg34/scenery_art/pillar_frame_6_object.inc"
};
static Sint16 pillar_frame_5_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_5_polygons.inc"
};
static Sint32 pillar_frame_5_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_5_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_5_model = {
#include "stg34/scenery_art/pillar_frame_5_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_5_object = {
#include "stg34/scenery_art/pillar_frame_5_object.inc"
};
static Sint16 pillar_frame_4_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_4_polygons.inc"
};
static Sint32 pillar_frame_4_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_4_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_4_model = {
#include "stg34/scenery_art/pillar_frame_4_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_4_object = {
#include "stg34/scenery_art/pillar_frame_4_object.inc"
};
static Sint16 pillar_frame_3_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_3_polygons.inc"
};
static Sint32 pillar_frame_3_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_3_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_3_model = {
#include "stg34/scenery_art/pillar_frame_3_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_3_object = {
#include "stg34/scenery_art/pillar_frame_3_object.inc"
};
static Sint16 pillar_frame_2_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_2_polygons.inc"
};
static Sint32 pillar_frame_2_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_2_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_2_model = {
#include "stg34/scenery_art/pillar_frame_2_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_2_object = {
#include "stg34/scenery_art/pillar_frame_2_object.inc"
};
static Sint16 pillar_frame_1_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_1_polygons.inc"
};
static Sint32 pillar_frame_1_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_1_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_1_model = {
#include "stg34/scenery_art/pillar_frame_1_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_1_object = {
#include "stg34/scenery_art/pillar_frame_1_object.inc"
};
static Sint16 pillar_frame_15_polygons[271] = {
#include "stg34/scenery_art/pillar_frame_15_polygons.inc"
};
static Sint32 pillar_frame_15_vertices[487] = {
#include "stg34/scenery_art/pillar_frame_15_vertices.inc"
};
static NJS_CNK_MODEL pillar_frame_15_model = {
#include "stg34/scenery_art/pillar_frame_15_model.inc"
};
static NJS_CNK_OBJECT pillar_frame_15_object = {
#include "stg34/scenery_art/pillar_frame_15_object.inc"
};
static NJS_CNK_OBJECT pillar_sequence_part_0 = {
#include "stg34/scenery_art/pillar_sequence_part_0.inc"
};
static NJS_TEXNAME water_unit_textures_names[1] = {
#include "stg34/scenery_art/water_unit_textures_names.inc"
};
NJS_TEXLIST water_unit_textures = {
#include "stg34/scenery_art/water_unit_textures.inc"
};
static Sint16 water_unit_frame_15_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_15_polygons.inc"
};
static Sint32 water_unit_frame_15_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_15_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_15_model = {
#include "stg34/scenery_art/water_unit_frame_15_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_15_object = {
#include "stg34/scenery_art/water_unit_frame_15_object.inc"
};
static Sint16 water_unit_frame_14_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_14_polygons.inc"
};
static Sint32 water_unit_frame_14_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_14_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_14_model = {
#include "stg34/scenery_art/water_unit_frame_14_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_14_object = {
#include "stg34/scenery_art/water_unit_frame_14_object.inc"
};
static Sint16 water_unit_frame_13_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_13_polygons.inc"
};
static Sint32 water_unit_frame_13_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_13_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_13_model = {
#include "stg34/scenery_art/water_unit_frame_13_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_13_object = {
#include "stg34/scenery_art/water_unit_frame_13_object.inc"
};
static Sint16 water_unit_frame_12_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_12_polygons.inc"
};
static Sint32 water_unit_frame_12_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_12_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_12_model = {
#include "stg34/scenery_art/water_unit_frame_12_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_12_object = {
#include "stg34/scenery_art/water_unit_frame_12_object.inc"
};
static Sint16 water_unit_frame_11_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_11_polygons.inc"
};
static Sint32 water_unit_frame_11_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_11_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_11_model = {
#include "stg34/scenery_art/water_unit_frame_11_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_11_object = {
#include "stg34/scenery_art/water_unit_frame_11_object.inc"
};
static Sint16 water_unit_frame_10_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_10_polygons.inc"
};
static Sint32 water_unit_frame_10_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_10_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_10_model = {
#include "stg34/scenery_art/water_unit_frame_10_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_10_object = {
#include "stg34/scenery_art/water_unit_frame_10_object.inc"
};
static Sint16 water_unit_frame_9_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_9_polygons.inc"
};
static Sint32 water_unit_frame_9_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_9_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_9_model = {
#include "stg34/scenery_art/water_unit_frame_9_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_9_object = {
#include "stg34/scenery_art/water_unit_frame_9_object.inc"
};
static Sint16 water_unit_frame_0_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_0_polygons.inc"
};
static Sint32 water_unit_frame_0_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_0_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_0_model = {
#include "stg34/scenery_art/water_unit_frame_0_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_0_object = {
#include "stg34/scenery_art/water_unit_frame_0_object.inc"
};
static Sint16 water_unit_frame_7_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_7_polygons.inc"
};
static Sint32 water_unit_frame_7_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_7_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_7_model = {
#include "stg34/scenery_art/water_unit_frame_7_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_7_object = {
#include "stg34/scenery_art/water_unit_frame_7_object.inc"
};
static Sint16 water_unit_frame_6_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_6_polygons.inc"
};
static Sint32 water_unit_frame_6_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_6_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_6_model = {
#include "stg34/scenery_art/water_unit_frame_6_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_6_object = {
#include "stg34/scenery_art/water_unit_frame_6_object.inc"
};
static Sint16 water_unit_frame_5_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_5_polygons.inc"
};
static Sint32 water_unit_frame_5_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_5_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_5_model = {
#include "stg34/scenery_art/water_unit_frame_5_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_5_object = {
#include "stg34/scenery_art/water_unit_frame_5_object.inc"
};
static Sint16 water_unit_frame_4_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_4_polygons.inc"
};
static Sint32 water_unit_frame_4_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_4_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_4_model = {
#include "stg34/scenery_art/water_unit_frame_4_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_4_object = {
#include "stg34/scenery_art/water_unit_frame_4_object.inc"
};
static Sint16 water_unit_frame_3_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_3_polygons.inc"
};
static Sint32 water_unit_frame_3_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_3_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_3_model = {
#include "stg34/scenery_art/water_unit_frame_3_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_3_object = {
#include "stg34/scenery_art/water_unit_frame_3_object.inc"
};
static Sint16 water_unit_frame_2_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_2_polygons.inc"
};
static Sint32 water_unit_frame_2_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_2_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_2_model = {
#include "stg34/scenery_art/water_unit_frame_2_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_2_object = {
#include "stg34/scenery_art/water_unit_frame_2_object.inc"
};
static Sint16 water_unit_frame_1_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_1_polygons.inc"
};
static Sint32 water_unit_frame_1_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_1_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_1_model = {
#include "stg34/scenery_art/water_unit_frame_1_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_1_object = {
#include "stg34/scenery_art/water_unit_frame_1_object.inc"
};
static Sint16 water_unit_frame_8_polygons[155] = {
#include "stg34/scenery_art/water_unit_frame_8_polygons.inc"
};
static Sint32 water_unit_frame_8_vertices[107] = {
#include "stg34/scenery_art/water_unit_frame_8_vertices.inc"
};
static NJS_CNK_MODEL water_unit_frame_8_model = {
#include "stg34/scenery_art/water_unit_frame_8_model.inc"
};
static NJS_CNK_OBJECT water_unit_frame_8_object = {
#include "stg34/scenery_art/water_unit_frame_8_object.inc"
};
static NJS_CNK_OBJECT water_unit_sequence_part_0 = {
#include "stg34/scenery_art/water_unit_sequence_part_0.inc"
};
static NJS_TEXNAME upper_channel_textures_names[1] = {
#include "stg34/scenery_art/upper_channel_textures_names.inc"
};
NJS_TEXLIST upper_channel_textures = {
#include "stg34/scenery_art/upper_channel_textures.inc"
};
static Sint16 upper_channel_frame_15_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_15_polygons.inc"
};
static Sint32 upper_channel_frame_15_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_15_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_15_model = {
#include "stg34/scenery_art/upper_channel_frame_15_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_15_object = {
#include "stg34/scenery_art/upper_channel_frame_15_object.inc"
};
static Sint16 upper_channel_frame_14_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_14_polygons.inc"
};
static Sint32 upper_channel_frame_14_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_14_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_14_model = {
#include "stg34/scenery_art/upper_channel_frame_14_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_14_object = {
#include "stg34/scenery_art/upper_channel_frame_14_object.inc"
};
static Sint16 upper_channel_frame_13_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_13_polygons.inc"
};
static Sint32 upper_channel_frame_13_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_13_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_13_model = {
#include "stg34/scenery_art/upper_channel_frame_13_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_13_object = {
#include "stg34/scenery_art/upper_channel_frame_13_object.inc"
};
static Sint16 upper_channel_frame_12_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_12_polygons.inc"
};
static Sint32 upper_channel_frame_12_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_12_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_12_model = {
#include "stg34/scenery_art/upper_channel_frame_12_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_12_object = {
#include "stg34/scenery_art/upper_channel_frame_12_object.inc"
};
static Sint16 upper_channel_frame_11_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_11_polygons.inc"
};
static Sint32 upper_channel_frame_11_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_11_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_11_model = {
#include "stg34/scenery_art/upper_channel_frame_11_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_11_object = {
#include "stg34/scenery_art/upper_channel_frame_11_object.inc"
};
static Sint16 upper_channel_frame_10_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_10_polygons.inc"
};
static Sint32 upper_channel_frame_10_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_10_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_10_model = {
#include "stg34/scenery_art/upper_channel_frame_10_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_10_object = {
#include "stg34/scenery_art/upper_channel_frame_10_object.inc"
};
static Sint16 upper_channel_frame_9_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_9_polygons.inc"
};
static Sint32 upper_channel_frame_9_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_9_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_9_model = {
#include "stg34/scenery_art/upper_channel_frame_9_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_9_object = {
#include "stg34/scenery_art/upper_channel_frame_9_object.inc"
};
static Sint16 upper_channel_frame_0_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_0_polygons.inc"
};
static Sint32 upper_channel_frame_0_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_0_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_0_model = {
#include "stg34/scenery_art/upper_channel_frame_0_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_0_object = {
#include "stg34/scenery_art/upper_channel_frame_0_object.inc"
};
static Sint16 upper_channel_frame_7_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_7_polygons.inc"
};
static Sint32 upper_channel_frame_7_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_7_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_7_model = {
#include "stg34/scenery_art/upper_channel_frame_7_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_7_object = {
#include "stg34/scenery_art/upper_channel_frame_7_object.inc"
};
static Sint16 upper_channel_frame_6_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_6_polygons.inc"
};
static Sint32 upper_channel_frame_6_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_6_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_6_model = {
#include "stg34/scenery_art/upper_channel_frame_6_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_6_object = {
#include "stg34/scenery_art/upper_channel_frame_6_object.inc"
};
static Sint16 upper_channel_frame_5_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_5_polygons.inc"
};
static Sint32 upper_channel_frame_5_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_5_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_5_model = {
#include "stg34/scenery_art/upper_channel_frame_5_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_5_object = {
#include "stg34/scenery_art/upper_channel_frame_5_object.inc"
};
static Sint16 upper_channel_frame_4_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_4_polygons.inc"
};
static Sint32 upper_channel_frame_4_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_4_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_4_model = {
#include "stg34/scenery_art/upper_channel_frame_4_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_4_object = {
#include "stg34/scenery_art/upper_channel_frame_4_object.inc"
};
static Sint16 upper_channel_frame_3_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_3_polygons.inc"
};
static Sint32 upper_channel_frame_3_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_3_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_3_model = {
#include "stg34/scenery_art/upper_channel_frame_3_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_3_object = {
#include "stg34/scenery_art/upper_channel_frame_3_object.inc"
};
static Sint16 upper_channel_frame_2_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_2_polygons.inc"
};
static Sint32 upper_channel_frame_2_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_2_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_2_model = {
#include "stg34/scenery_art/upper_channel_frame_2_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_2_object = {
#include "stg34/scenery_art/upper_channel_frame_2_object.inc"
};
static Sint16 upper_channel_frame_1_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_1_polygons.inc"
};
static Sint32 upper_channel_frame_1_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_1_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_1_model = {
#include "stg34/scenery_art/upper_channel_frame_1_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_1_object = {
#include "stg34/scenery_art/upper_channel_frame_1_object.inc"
};
static Sint16 upper_channel_frame_8_polygons[155] = {
#include "stg34/scenery_art/upper_channel_frame_8_polygons.inc"
};
static Sint32 upper_channel_frame_8_vertices[107] = {
#include "stg34/scenery_art/upper_channel_frame_8_vertices.inc"
};
static NJS_CNK_MODEL upper_channel_frame_8_model = {
#include "stg34/scenery_art/upper_channel_frame_8_model.inc"
};
static NJS_CNK_OBJECT upper_channel_frame_8_object = {
#include "stg34/scenery_art/upper_channel_frame_8_object.inc"
};
static NJS_CNK_OBJECT upper_channel_sequence_part_0 = {
#include "stg34/scenery_art/upper_channel_sequence_part_0.inc"
};
static NJS_TEXNAME middle_channel_textures_names[1] = {
#include "stg34/scenery_art/middle_channel_textures_names.inc"
};
NJS_TEXLIST middle_channel_textures = {
#include "stg34/scenery_art/middle_channel_textures.inc"
};
static Sint16 middle_channel_frame_15_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_15_polygons.inc"
};
static Sint32 middle_channel_frame_15_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_15_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_15_model = {
#include "stg34/scenery_art/middle_channel_frame_15_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_15_object = {
#include "stg34/scenery_art/middle_channel_frame_15_object.inc"
};
static Sint16 middle_channel_frame_14_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_14_polygons.inc"
};
static Sint32 middle_channel_frame_14_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_14_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_14_model = {
#include "stg34/scenery_art/middle_channel_frame_14_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_14_object = {
#include "stg34/scenery_art/middle_channel_frame_14_object.inc"
};
static Sint16 middle_channel_frame_13_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_13_polygons.inc"
};
static Sint32 middle_channel_frame_13_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_13_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_13_model = {
#include "stg34/scenery_art/middle_channel_frame_13_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_13_object = {
#include "stg34/scenery_art/middle_channel_frame_13_object.inc"
};
static Sint16 middle_channel_frame_12_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_12_polygons.inc"
};
static Sint32 middle_channel_frame_12_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_12_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_12_model = {
#include "stg34/scenery_art/middle_channel_frame_12_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_12_object = {
#include "stg34/scenery_art/middle_channel_frame_12_object.inc"
};
static Sint16 middle_channel_frame_11_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_11_polygons.inc"
};
static Sint32 middle_channel_frame_11_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_11_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_11_model = {
#include "stg34/scenery_art/middle_channel_frame_11_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_11_object = {
#include "stg34/scenery_art/middle_channel_frame_11_object.inc"
};
static Sint16 middle_channel_frame_10_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_10_polygons.inc"
};
static Sint32 middle_channel_frame_10_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_10_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_10_model = {
#include "stg34/scenery_art/middle_channel_frame_10_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_10_object = {
#include "stg34/scenery_art/middle_channel_frame_10_object.inc"
};
static Sint16 middle_channel_frame_9_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_9_polygons.inc"
};
static Sint32 middle_channel_frame_9_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_9_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_9_model = {
#include "stg34/scenery_art/middle_channel_frame_9_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_9_object = {
#include "stg34/scenery_art/middle_channel_frame_9_object.inc"
};
static Sint16 middle_channel_frame_0_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_0_polygons.inc"
};
static Sint32 middle_channel_frame_0_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_0_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_0_model = {
#include "stg34/scenery_art/middle_channel_frame_0_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_0_object = {
#include "stg34/scenery_art/middle_channel_frame_0_object.inc"
};
static Sint16 middle_channel_frame_7_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_7_polygons.inc"
};
static Sint32 middle_channel_frame_7_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_7_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_7_model = {
#include "stg34/scenery_art/middle_channel_frame_7_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_7_object = {
#include "stg34/scenery_art/middle_channel_frame_7_object.inc"
};
static Sint16 middle_channel_frame_6_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_6_polygons.inc"
};
static Sint32 middle_channel_frame_6_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_6_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_6_model = {
#include "stg34/scenery_art/middle_channel_frame_6_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_6_object = {
#include "stg34/scenery_art/middle_channel_frame_6_object.inc"
};
static Sint16 middle_channel_frame_5_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_5_polygons.inc"
};
static Sint32 middle_channel_frame_5_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_5_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_5_model = {
#include "stg34/scenery_art/middle_channel_frame_5_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_5_object = {
#include "stg34/scenery_art/middle_channel_frame_5_object.inc"
};
static Sint16 middle_channel_frame_4_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_4_polygons.inc"
};
static Sint32 middle_channel_frame_4_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_4_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_4_model = {
#include "stg34/scenery_art/middle_channel_frame_4_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_4_object = {
#include "stg34/scenery_art/middle_channel_frame_4_object.inc"
};
static Sint16 middle_channel_frame_3_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_3_polygons.inc"
};
static Sint32 middle_channel_frame_3_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_3_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_3_model = {
#include "stg34/scenery_art/middle_channel_frame_3_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_3_object = {
#include "stg34/scenery_art/middle_channel_frame_3_object.inc"
};
static Sint16 middle_channel_frame_2_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_2_polygons.inc"
};
static Sint32 middle_channel_frame_2_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_2_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_2_model = {
#include "stg34/scenery_art/middle_channel_frame_2_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_2_object = {
#include "stg34/scenery_art/middle_channel_frame_2_object.inc"
};
static Sint16 middle_channel_frame_1_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_1_polygons.inc"
};
static Sint32 middle_channel_frame_1_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_1_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_1_model = {
#include "stg34/scenery_art/middle_channel_frame_1_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_1_object = {
#include "stg34/scenery_art/middle_channel_frame_1_object.inc"
};
static Sint16 middle_channel_frame_8_polygons[155] = {
#include "stg34/scenery_art/middle_channel_frame_8_polygons.inc"
};
static Sint32 middle_channel_frame_8_vertices[107] = {
#include "stg34/scenery_art/middle_channel_frame_8_vertices.inc"
};
static NJS_CNK_MODEL middle_channel_frame_8_model = {
#include "stg34/scenery_art/middle_channel_frame_8_model.inc"
};
static NJS_CNK_OBJECT middle_channel_frame_8_object = {
#include "stg34/scenery_art/middle_channel_frame_8_object.inc"
};
static NJS_CNK_OBJECT middle_channel_sequence_part_0 = {
#include "stg34/scenery_art/middle_channel_sequence_part_0.inc"
};
static NJS_TEXNAME lower_channel_textures_names[1] = {
#include "stg34/scenery_art/lower_channel_textures_names.inc"
};
NJS_TEXLIST lower_channel_textures = {
#include "stg34/scenery_art/lower_channel_textures.inc"
};
static Sint16 lower_channel_frame_15_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_15_polygons.inc"
};
static Sint32 lower_channel_frame_15_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_15_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_15_model = {
#include "stg34/scenery_art/lower_channel_frame_15_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_15_object = {
#include "stg34/scenery_art/lower_channel_frame_15_object.inc"
};
static Sint16 lower_channel_frame_14_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_14_polygons.inc"
};
static Sint32 lower_channel_frame_14_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_14_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_14_model = {
#include "stg34/scenery_art/lower_channel_frame_14_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_14_object = {
#include "stg34/scenery_art/lower_channel_frame_14_object.inc"
};
static Sint16 lower_channel_frame_13_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_13_polygons.inc"
};
static Sint32 lower_channel_frame_13_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_13_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_13_model = {
#include "stg34/scenery_art/lower_channel_frame_13_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_13_object = {
#include "stg34/scenery_art/lower_channel_frame_13_object.inc"
};
static Sint16 lower_channel_frame_12_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_12_polygons.inc"
};
static Sint32 lower_channel_frame_12_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_12_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_12_model = {
#include "stg34/scenery_art/lower_channel_frame_12_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_12_object = {
#include "stg34/scenery_art/lower_channel_frame_12_object.inc"
};
static Sint16 lower_channel_frame_11_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_11_polygons.inc"
};
static Sint32 lower_channel_frame_11_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_11_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_11_model = {
#include "stg34/scenery_art/lower_channel_frame_11_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_11_object = {
#include "stg34/scenery_art/lower_channel_frame_11_object.inc"
};
static Sint16 lower_channel_frame_10_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_10_polygons.inc"
};
static Sint32 lower_channel_frame_10_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_10_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_10_model = {
#include "stg34/scenery_art/lower_channel_frame_10_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_10_object = {
#include "stg34/scenery_art/lower_channel_frame_10_object.inc"
};
static Sint16 lower_channel_frame_9_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_9_polygons.inc"
};
static Sint32 lower_channel_frame_9_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_9_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_9_model = {
#include "stg34/scenery_art/lower_channel_frame_9_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_9_object = {
#include "stg34/scenery_art/lower_channel_frame_9_object.inc"
};
static Sint16 lower_channel_frame_0_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_0_polygons.inc"
};
static Sint32 lower_channel_frame_0_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_0_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_0_model = {
#include "stg34/scenery_art/lower_channel_frame_0_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_0_object = {
#include "stg34/scenery_art/lower_channel_frame_0_object.inc"
};
static Sint16 lower_channel_frame_7_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_7_polygons.inc"
};
static Sint32 lower_channel_frame_7_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_7_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_7_model = {
#include "stg34/scenery_art/lower_channel_frame_7_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_7_object = {
#include "stg34/scenery_art/lower_channel_frame_7_object.inc"
};
static Sint16 lower_channel_frame_6_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_6_polygons.inc"
};
static Sint32 lower_channel_frame_6_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_6_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_6_model = {
#include "stg34/scenery_art/lower_channel_frame_6_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_6_object = {
#include "stg34/scenery_art/lower_channel_frame_6_object.inc"
};
static Sint16 lower_channel_frame_5_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_5_polygons.inc"
};
static Sint32 lower_channel_frame_5_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_5_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_5_model = {
#include "stg34/scenery_art/lower_channel_frame_5_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_5_object = {
#include "stg34/scenery_art/lower_channel_frame_5_object.inc"
};
static Sint16 lower_channel_frame_4_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_4_polygons.inc"
};
static Sint32 lower_channel_frame_4_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_4_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_4_model = {
#include "stg34/scenery_art/lower_channel_frame_4_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_4_object = {
#include "stg34/scenery_art/lower_channel_frame_4_object.inc"
};
static Sint16 lower_channel_frame_3_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_3_polygons.inc"
};
static Sint32 lower_channel_frame_3_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_3_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_3_model = {
#include "stg34/scenery_art/lower_channel_frame_3_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_3_object = {
#include "stg34/scenery_art/lower_channel_frame_3_object.inc"
};
static Sint16 lower_channel_frame_2_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_2_polygons.inc"
};
static Sint32 lower_channel_frame_2_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_2_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_2_model = {
#include "stg34/scenery_art/lower_channel_frame_2_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_2_object = {
#include "stg34/scenery_art/lower_channel_frame_2_object.inc"
};
static Sint16 lower_channel_frame_1_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_1_polygons.inc"
};
static Sint32 lower_channel_frame_1_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_1_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_1_model = {
#include "stg34/scenery_art/lower_channel_frame_1_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_1_object = {
#include "stg34/scenery_art/lower_channel_frame_1_object.inc"
};
static Sint16 lower_channel_frame_8_polygons[77] = {
#include "stg34/scenery_art/lower_channel_frame_8_polygons.inc"
};
static Sint32 lower_channel_frame_8_vertices[51] = {
#include "stg34/scenery_art/lower_channel_frame_8_vertices.inc"
};
static NJS_CNK_MODEL lower_channel_frame_8_model = {
#include "stg34/scenery_art/lower_channel_frame_8_model.inc"
};
static NJS_CNK_OBJECT lower_channel_frame_8_object = {
#include "stg34/scenery_art/lower_channel_frame_8_object.inc"
};
static NJS_CNK_OBJECT lower_channel_sequence_part_0 = {
#include "stg34/scenery_art/lower_channel_sequence_part_0.inc"
};
static NJS_TEXNAME waterfall_archive_textures_names[1] = {
#include "stg34/scenery_art/waterfall_archive_textures_names.inc"
};
static NJS_TEXLIST waterfall_archive_textures = {
#include "stg34/scenery_art/waterfall_archive_textures.inc"
};
static Sint16 waterfall_frame_59_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_59_polygons.inc"
};
static Sint32 waterfall_frame_59_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_59_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_59_model = {
#include "stg34/scenery_art/waterfall_frame_59_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_59_object = {
#include "stg34/scenery_art/waterfall_frame_59_object.inc"
};
static Sint16 waterfall_frame_58_polygons[541] = {
#include "stg34/scenery_art/waterfall_frame_58_polygons.inc"
};
static Sint32 waterfall_frame_58_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_58_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_58_model = {
#include "stg34/scenery_art/waterfall_frame_58_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_58_object = {
#include "stg34/scenery_art/waterfall_frame_58_object.inc"
};
static Sint16 waterfall_frame_57_polygons[549] = {
#include "stg34/scenery_art/waterfall_frame_57_polygons.inc"
};
static Sint32 waterfall_frame_57_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_57_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_57_model = {
#include "stg34/scenery_art/waterfall_frame_57_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_57_object = {
#include "stg34/scenery_art/waterfall_frame_57_object.inc"
};
static Sint16 waterfall_frame_56_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_56_polygons.inc"
};
static Sint32 waterfall_frame_56_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_56_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_56_model = {
#include "stg34/scenery_art/waterfall_frame_56_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_56_object = {
#include "stg34/scenery_art/waterfall_frame_56_object.inc"
};
static Sint16 waterfall_frame_55_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_55_polygons.inc"
};
static Sint32 waterfall_frame_55_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_55_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_55_model = {
#include "stg34/scenery_art/waterfall_frame_55_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_55_object = {
#include "stg34/scenery_art/waterfall_frame_55_object.inc"
};
static Sint16 waterfall_frame_54_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_54_polygons.inc"
};
static Sint32 waterfall_frame_54_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_54_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_54_model = {
#include "stg34/scenery_art/waterfall_frame_54_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_54_object = {
#include "stg34/scenery_art/waterfall_frame_54_object.inc"
};
static Sint16 waterfall_frame_53_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_53_polygons.inc"
};
static Sint32 waterfall_frame_53_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_53_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_53_model = {
#include "stg34/scenery_art/waterfall_frame_53_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_53_object = {
#include "stg34/scenery_art/waterfall_frame_53_object.inc"
};
static Sint16 waterfall_frame_52_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_52_polygons.inc"
};
static Sint32 waterfall_frame_52_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_52_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_52_model = {
#include "stg34/scenery_art/waterfall_frame_52_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_52_object = {
#include "stg34/scenery_art/waterfall_frame_52_object.inc"
};
static Sint16 waterfall_frame_51_polygons[521] = {
#include "stg34/scenery_art/waterfall_frame_51_polygons.inc"
};
static Sint32 waterfall_frame_51_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_51_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_51_model = {
#include "stg34/scenery_art/waterfall_frame_51_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_51_object = {
#include "stg34/scenery_art/waterfall_frame_51_object.inc"
};
static Sint16 waterfall_frame_50_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_50_polygons.inc"
};
static Sint32 waterfall_frame_50_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_50_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_50_model = {
#include "stg34/scenery_art/waterfall_frame_50_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_50_object = {
#include "stg34/scenery_art/waterfall_frame_50_object.inc"
};
static Sint16 waterfall_frame_49_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_49_polygons.inc"
};
static Sint32 waterfall_frame_49_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_49_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_49_model = {
#include "stg34/scenery_art/waterfall_frame_49_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_49_object = {
#include "stg34/scenery_art/waterfall_frame_49_object.inc"
};
static Sint16 waterfall_frame_48_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_48_polygons.inc"
};
static Sint32 waterfall_frame_48_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_48_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_48_model = {
#include "stg34/scenery_art/waterfall_frame_48_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_48_object = {
#include "stg34/scenery_art/waterfall_frame_48_object.inc"
};
static Sint16 waterfall_frame_47_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_47_polygons.inc"
};
static Sint32 waterfall_frame_47_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_47_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_47_model = {
#include "stg34/scenery_art/waterfall_frame_47_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_47_object = {
#include "stg34/scenery_art/waterfall_frame_47_object.inc"
};
static Sint16 waterfall_frame_46_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_46_polygons.inc"
};
static Sint32 waterfall_frame_46_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_46_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_46_model = {
#include "stg34/scenery_art/waterfall_frame_46_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_46_object = {
#include "stg34/scenery_art/waterfall_frame_46_object.inc"
};
static Sint16 waterfall_frame_45_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_45_polygons.inc"
};
static Sint32 waterfall_frame_45_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_45_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_45_model = {
#include "stg34/scenery_art/waterfall_frame_45_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_45_object = {
#include "stg34/scenery_art/waterfall_frame_45_object.inc"
};
static Sint16 waterfall_frame_44_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_44_polygons.inc"
};
static Sint32 waterfall_frame_44_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_44_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_44_model = {
#include "stg34/scenery_art/waterfall_frame_44_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_44_object = {
#include "stg34/scenery_art/waterfall_frame_44_object.inc"
};
static Sint16 waterfall_frame_43_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_43_polygons.inc"
};
static Sint32 waterfall_frame_43_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_43_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_43_model = {
#include "stg34/scenery_art/waterfall_frame_43_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_43_object = {
#include "stg34/scenery_art/waterfall_frame_43_object.inc"
};
static Sint16 waterfall_frame_42_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_42_polygons.inc"
};
static Sint32 waterfall_frame_42_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_42_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_42_model = {
#include "stg34/scenery_art/waterfall_frame_42_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_42_object = {
#include "stg34/scenery_art/waterfall_frame_42_object.inc"
};
static Sint16 waterfall_frame_41_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_41_polygons.inc"
};
static Sint32 waterfall_frame_41_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_41_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_41_model = {
#include "stg34/scenery_art/waterfall_frame_41_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_41_object = {
#include "stg34/scenery_art/waterfall_frame_41_object.inc"
};
static Sint16 waterfall_frame_40_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_40_polygons.inc"
};
static Sint32 waterfall_frame_40_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_40_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_40_model = {
#include "stg34/scenery_art/waterfall_frame_40_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_40_object = {
#include "stg34/scenery_art/waterfall_frame_40_object.inc"
};
static Sint16 waterfall_frame_39_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_39_polygons.inc"
};
static Sint32 waterfall_frame_39_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_39_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_39_model = {
#include "stg34/scenery_art/waterfall_frame_39_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_39_object = {
#include "stg34/scenery_art/waterfall_frame_39_object.inc"
};
static Sint16 waterfall_frame_38_polygons[521] = {
#include "stg34/scenery_art/waterfall_frame_38_polygons.inc"
};
static Sint32 waterfall_frame_38_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_38_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_38_model = {
#include "stg34/scenery_art/waterfall_frame_38_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_38_object = {
#include "stg34/scenery_art/waterfall_frame_38_object.inc"
};
static Sint16 waterfall_frame_37_polygons[541] = {
#include "stg34/scenery_art/waterfall_frame_37_polygons.inc"
};
static Sint32 waterfall_frame_37_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_37_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_37_model = {
#include "stg34/scenery_art/waterfall_frame_37_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_37_object = {
#include "stg34/scenery_art/waterfall_frame_37_object.inc"
};
static Sint16 waterfall_frame_36_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_36_polygons.inc"
};
static Sint32 waterfall_frame_36_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_36_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_36_model = {
#include "stg34/scenery_art/waterfall_frame_36_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_36_object = {
#include "stg34/scenery_art/waterfall_frame_36_object.inc"
};
static Sint16 waterfall_frame_35_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_35_polygons.inc"
};
static Sint32 waterfall_frame_35_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_35_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_35_model = {
#include "stg34/scenery_art/waterfall_frame_35_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_35_object = {
#include "stg34/scenery_art/waterfall_frame_35_object.inc"
};
static Sint16 waterfall_frame_34_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_34_polygons.inc"
};
static Sint32 waterfall_frame_34_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_34_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_34_model = {
#include "stg34/scenery_art/waterfall_frame_34_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_34_object = {
#include "stg34/scenery_art/waterfall_frame_34_object.inc"
};
static Sint16 waterfall_frame_33_polygons[521] = {
#include "stg34/scenery_art/waterfall_frame_33_polygons.inc"
};
static Sint32 waterfall_frame_33_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_33_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_33_model = {
#include "stg34/scenery_art/waterfall_frame_33_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_33_object = {
#include "stg34/scenery_art/waterfall_frame_33_object.inc"
};
static Sint16 waterfall_frame_32_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_32_polygons.inc"
};
static Sint32 waterfall_frame_32_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_32_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_32_model = {
#include "stg34/scenery_art/waterfall_frame_32_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_32_object = {
#include "stg34/scenery_art/waterfall_frame_32_object.inc"
};
static Sint16 waterfall_frame_31_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_31_polygons.inc"
};
static Sint32 waterfall_frame_31_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_31_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_31_model = {
#include "stg34/scenery_art/waterfall_frame_31_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_31_object = {
#include "stg34/scenery_art/waterfall_frame_31_object.inc"
};
static Sint16 waterfall_frame_0_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_0_polygons.inc"
};
static Sint32 waterfall_frame_0_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_0_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_0_model = {
#include "stg34/scenery_art/waterfall_frame_0_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_0_object = {
#include "stg34/scenery_art/waterfall_frame_0_object.inc"
};
static Sint16 waterfall_frame_29_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_29_polygons.inc"
};
static Sint32 waterfall_frame_29_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_29_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_29_model = {
#include "stg34/scenery_art/waterfall_frame_29_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_29_object = {
#include "stg34/scenery_art/waterfall_frame_29_object.inc"
};
static Sint16 waterfall_frame_28_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_28_polygons.inc"
};
static Sint32 waterfall_frame_28_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_28_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_28_model = {
#include "stg34/scenery_art/waterfall_frame_28_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_28_object = {
#include "stg34/scenery_art/waterfall_frame_28_object.inc"
};
static Sint16 waterfall_frame_27_polygons[541] = {
#include "stg34/scenery_art/waterfall_frame_27_polygons.inc"
};
static Sint32 waterfall_frame_27_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_27_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_27_model = {
#include "stg34/scenery_art/waterfall_frame_27_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_27_object = {
#include "stg34/scenery_art/waterfall_frame_27_object.inc"
};
static Sint16 waterfall_frame_26_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_26_polygons.inc"
};
static Sint32 waterfall_frame_26_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_26_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_26_model = {
#include "stg34/scenery_art/waterfall_frame_26_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_26_object = {
#include "stg34/scenery_art/waterfall_frame_26_object.inc"
};
static Sint16 waterfall_frame_25_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_25_polygons.inc"
};
static Sint32 waterfall_frame_25_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_25_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_25_model = {
#include "stg34/scenery_art/waterfall_frame_25_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_25_object = {
#include "stg34/scenery_art/waterfall_frame_25_object.inc"
};
static Sint16 waterfall_frame_24_polygons[541] = {
#include "stg34/scenery_art/waterfall_frame_24_polygons.inc"
};
static Sint32 waterfall_frame_24_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_24_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_24_model = {
#include "stg34/scenery_art/waterfall_frame_24_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_24_object = {
#include "stg34/scenery_art/waterfall_frame_24_object.inc"
};
static Sint16 waterfall_frame_23_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_23_polygons.inc"
};
static Sint32 waterfall_frame_23_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_23_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_23_model = {
#include "stg34/scenery_art/waterfall_frame_23_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_23_object = {
#include "stg34/scenery_art/waterfall_frame_23_object.inc"
};
static Sint16 waterfall_frame_22_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_22_polygons.inc"
};
static Sint32 waterfall_frame_22_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_22_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_22_model = {
#include "stg34/scenery_art/waterfall_frame_22_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_22_object = {
#include "stg34/scenery_art/waterfall_frame_22_object.inc"
};
static Sint16 waterfall_frame_21_polygons[555] = {
#include "stg34/scenery_art/waterfall_frame_21_polygons.inc"
};
static Sint32 waterfall_frame_21_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_21_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_21_model = {
#include "stg34/scenery_art/waterfall_frame_21_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_21_object = {
#include "stg34/scenery_art/waterfall_frame_21_object.inc"
};
static Sint16 waterfall_frame_20_polygons[555] = {
#include "stg34/scenery_art/waterfall_frame_20_polygons.inc"
};
static Sint32 waterfall_frame_20_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_20_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_20_model = {
#include "stg34/scenery_art/waterfall_frame_20_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_20_object = {
#include "stg34/scenery_art/waterfall_frame_20_object.inc"
};
static Sint16 waterfall_frame_19_polygons[555] = {
#include "stg34/scenery_art/waterfall_frame_19_polygons.inc"
};
static Sint32 waterfall_frame_19_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_19_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_19_model = {
#include "stg34/scenery_art/waterfall_frame_19_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_19_object = {
#include "stg34/scenery_art/waterfall_frame_19_object.inc"
};
static Sint16 waterfall_frame_18_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_18_polygons.inc"
};
static Sint32 waterfall_frame_18_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_18_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_18_model = {
#include "stg34/scenery_art/waterfall_frame_18_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_18_object = {
#include "stg34/scenery_art/waterfall_frame_18_object.inc"
};
static Sint16 waterfall_frame_17_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_17_polygons.inc"
};
static Sint32 waterfall_frame_17_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_17_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_17_model = {
#include "stg34/scenery_art/waterfall_frame_17_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_17_object = {
#include "stg34/scenery_art/waterfall_frame_17_object.inc"
};
static Sint16 waterfall_frame_16_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_16_polygons.inc"
};
static Sint32 waterfall_frame_16_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_16_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_16_model = {
#include "stg34/scenery_art/waterfall_frame_16_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_16_object = {
#include "stg34/scenery_art/waterfall_frame_16_object.inc"
};
static Sint16 waterfall_frame_15_polygons[527] = {
#include "stg34/scenery_art/waterfall_frame_15_polygons.inc"
};
static Sint32 waterfall_frame_15_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_15_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_15_model = {
#include "stg34/scenery_art/waterfall_frame_15_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_15_object = {
#include "stg34/scenery_art/waterfall_frame_15_object.inc"
};
static Sint16 waterfall_frame_14_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_14_polygons.inc"
};
static Sint32 waterfall_frame_14_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_14_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_14_model = {
#include "stg34/scenery_art/waterfall_frame_14_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_14_object = {
#include "stg34/scenery_art/waterfall_frame_14_object.inc"
};
static Sint16 waterfall_frame_13_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_13_polygons.inc"
};
static Sint32 waterfall_frame_13_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_13_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_13_model = {
#include "stg34/scenery_art/waterfall_frame_13_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_13_object = {
#include "stg34/scenery_art/waterfall_frame_13_object.inc"
};
static Sint16 waterfall_frame_12_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_12_polygons.inc"
};
static Sint32 waterfall_frame_12_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_12_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_12_model = {
#include "stg34/scenery_art/waterfall_frame_12_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_12_object = {
#include "stg34/scenery_art/waterfall_frame_12_object.inc"
};
static Sint16 waterfall_frame_11_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_11_polygons.inc"
};
static Sint32 waterfall_frame_11_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_11_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_11_model = {
#include "stg34/scenery_art/waterfall_frame_11_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_11_object = {
#include "stg34/scenery_art/waterfall_frame_11_object.inc"
};
static Sint16 waterfall_frame_10_polygons[521] = {
#include "stg34/scenery_art/waterfall_frame_10_polygons.inc"
};
static Sint32 waterfall_frame_10_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_10_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_10_model = {
#include "stg34/scenery_art/waterfall_frame_10_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_10_object = {
#include "stg34/scenery_art/waterfall_frame_10_object.inc"
};
static Sint16 waterfall_frame_9_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_9_polygons.inc"
};
static Sint32 waterfall_frame_9_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_9_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_9_model = {
#include "stg34/scenery_art/waterfall_frame_9_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_9_object = {
#include "stg34/scenery_art/waterfall_frame_9_object.inc"
};
static Sint16 waterfall_frame_8_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_8_polygons.inc"
};
static Sint32 waterfall_frame_8_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_8_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_8_model = {
#include "stg34/scenery_art/waterfall_frame_8_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_8_object = {
#include "stg34/scenery_art/waterfall_frame_8_object.inc"
};
static Sint16 waterfall_frame_7_polygons[535] = {
#include "stg34/scenery_art/waterfall_frame_7_polygons.inc"
};
static Sint32 waterfall_frame_7_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_7_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_7_model = {
#include "stg34/scenery_art/waterfall_frame_7_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_7_object = {
#include "stg34/scenery_art/waterfall_frame_7_object.inc"
};
static Sint16 waterfall_frame_6_polygons[521] = {
#include "stg34/scenery_art/waterfall_frame_6_polygons.inc"
};
static Sint32 waterfall_frame_6_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_6_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_6_model = {
#include "stg34/scenery_art/waterfall_frame_6_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_6_object = {
#include "stg34/scenery_art/waterfall_frame_6_object.inc"
};
static Sint16 waterfall_frame_5_polygons[555] = {
#include "stg34/scenery_art/waterfall_frame_5_polygons.inc"
};
static Sint32 waterfall_frame_5_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_5_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_5_model = {
#include "stg34/scenery_art/waterfall_frame_5_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_5_object = {
#include "stg34/scenery_art/waterfall_frame_5_object.inc"
};
static Sint16 waterfall_frame_4_polygons[555] = {
#include "stg34/scenery_art/waterfall_frame_4_polygons.inc"
};
static Sint32 waterfall_frame_4_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_4_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_4_model = {
#include "stg34/scenery_art/waterfall_frame_4_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_4_object = {
#include "stg34/scenery_art/waterfall_frame_4_object.inc"
};
static Sint16 waterfall_frame_3_polygons[507] = {
#include "stg34/scenery_art/waterfall_frame_3_polygons.inc"
};
static Sint32 waterfall_frame_3_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_3_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_3_model = {
#include "stg34/scenery_art/waterfall_frame_3_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_3_object = {
#include "stg34/scenery_art/waterfall_frame_3_object.inc"
};
static Sint16 waterfall_frame_2_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_2_polygons.inc"
};
static Sint32 waterfall_frame_2_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_2_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_2_model = {
#include "stg34/scenery_art/waterfall_frame_2_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_2_object = {
#include "stg34/scenery_art/waterfall_frame_2_object.inc"
};
static Sint16 waterfall_frame_1_polygons[513] = {
#include "stg34/scenery_art/waterfall_frame_1_polygons.inc"
};
static Sint32 waterfall_frame_1_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_1_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_1_model = {
#include "stg34/scenery_art/waterfall_frame_1_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_1_object = {
#include "stg34/scenery_art/waterfall_frame_1_object.inc"
};
static Sint16 waterfall_frame_30_polygons[541] = {
#include "stg34/scenery_art/waterfall_frame_30_polygons.inc"
};
static Sint32 waterfall_frame_30_vertices[323] = {
#include "stg34/scenery_art/waterfall_frame_30_vertices.inc"
};
static NJS_CNK_MODEL waterfall_frame_30_model = {
#include "stg34/scenery_art/waterfall_frame_30_model.inc"
};
static NJS_CNK_OBJECT waterfall_frame_30_object = {
#include "stg34/scenery_art/waterfall_frame_30_object.inc"
};
static NJS_CNK_OBJECT waterfall_sequence_part_0 = {
#include "stg34/scenery_art/waterfall_sequence_part_0.inc"
};
static NJS_TEXNAME falling_water_archive_textures_names[1] = {
#include "stg34/scenery_art/falling_water_archive_textures_names.inc"
};
static NJS_TEXLIST falling_water_archive_textures = {
#include "stg34/scenery_art/falling_water_archive_textures.inc"
};
static Sint16 falling_water_frame_29_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_29_polygons.inc"
};
static Sint32 falling_water_frame_29_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_29_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_29_model = {
#include "stg34/scenery_art/falling_water_frame_29_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_29_object = {
#include "stg34/scenery_art/falling_water_frame_29_object.inc"
};
static Sint16 falling_water_frame_28_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_28_polygons.inc"
};
static Sint32 falling_water_frame_28_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_28_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_28_model = {
#include "stg34/scenery_art/falling_water_frame_28_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_28_object = {
#include "stg34/scenery_art/falling_water_frame_28_object.inc"
};
static Sint16 falling_water_frame_27_polygons[279] = {
#include "stg34/scenery_art/falling_water_frame_27_polygons.inc"
};
static Sint32 falling_water_frame_27_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_27_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_27_model = {
#include "stg34/scenery_art/falling_water_frame_27_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_27_object = {
#include "stg34/scenery_art/falling_water_frame_27_object.inc"
};
static Sint16 falling_water_frame_26_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_26_polygons.inc"
};
static Sint32 falling_water_frame_26_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_26_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_26_model = {
#include "stg34/scenery_art/falling_water_frame_26_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_26_object = {
#include "stg34/scenery_art/falling_water_frame_26_object.inc"
};
static Sint16 falling_water_frame_25_polygons[265] = {
#include "stg34/scenery_art/falling_water_frame_25_polygons.inc"
};
static Sint32 falling_water_frame_25_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_25_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_25_model = {
#include "stg34/scenery_art/falling_water_frame_25_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_25_object = {
#include "stg34/scenery_art/falling_water_frame_25_object.inc"
};
static Sint16 falling_water_frame_24_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_24_polygons.inc"
};
static Sint32 falling_water_frame_24_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_24_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_24_model = {
#include "stg34/scenery_art/falling_water_frame_24_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_24_object = {
#include "stg34/scenery_art/falling_water_frame_24_object.inc"
};
static Sint16 falling_water_frame_23_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_23_polygons.inc"
};
static Sint32 falling_water_frame_23_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_23_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_23_model = {
#include "stg34/scenery_art/falling_water_frame_23_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_23_object = {
#include "stg34/scenery_art/falling_water_frame_23_object.inc"
};
static Sint16 falling_water_frame_22_polygons[279] = {
#include "stg34/scenery_art/falling_water_frame_22_polygons.inc"
};
static Sint32 falling_water_frame_22_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_22_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_22_model = {
#include "stg34/scenery_art/falling_water_frame_22_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_22_object = {
#include "stg34/scenery_art/falling_water_frame_22_object.inc"
};
static Sint16 falling_water_frame_21_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_21_polygons.inc"
};
static Sint32 falling_water_frame_21_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_21_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_21_model = {
#include "stg34/scenery_art/falling_water_frame_21_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_21_object = {
#include "stg34/scenery_art/falling_water_frame_21_object.inc"
};
static Sint16 falling_water_frame_20_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_20_polygons.inc"
};
static Sint32 falling_water_frame_20_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_20_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_20_model = {
#include "stg34/scenery_art/falling_water_frame_20_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_20_object = {
#include "stg34/scenery_art/falling_water_frame_20_object.inc"
};
static Sint16 falling_water_frame_19_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_19_polygons.inc"
};
static Sint32 falling_water_frame_19_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_19_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_19_model = {
#include "stg34/scenery_art/falling_water_frame_19_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_19_object = {
#include "stg34/scenery_art/falling_water_frame_19_object.inc"
};
static Sint16 falling_water_frame_18_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_18_polygons.inc"
};
static Sint32 falling_water_frame_18_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_18_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_18_model = {
#include "stg34/scenery_art/falling_water_frame_18_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_18_object = {
#include "stg34/scenery_art/falling_water_frame_18_object.inc"
};
static Sint16 falling_water_frame_17_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_17_polygons.inc"
};
static Sint32 falling_water_frame_17_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_17_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_17_model = {
#include "stg34/scenery_art/falling_water_frame_17_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_17_object = {
#include "stg34/scenery_art/falling_water_frame_17_object.inc"
};
static Sint16 falling_water_frame_16_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_16_polygons.inc"
};
static Sint32 falling_water_frame_16_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_16_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_16_model = {
#include "stg34/scenery_art/falling_water_frame_16_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_16_object = {
#include "stg34/scenery_art/falling_water_frame_16_object.inc"
};
static Sint16 falling_water_frame_0_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_0_polygons.inc"
};
static Sint32 falling_water_frame_0_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_0_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_0_model = {
#include "stg34/scenery_art/falling_water_frame_0_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_0_object = {
#include "stg34/scenery_art/falling_water_frame_0_object.inc"
};
static Sint16 falling_water_frame_14_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_14_polygons.inc"
};
static Sint32 falling_water_frame_14_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_14_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_14_model = {
#include "stg34/scenery_art/falling_water_frame_14_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_14_object = {
#include "stg34/scenery_art/falling_water_frame_14_object.inc"
};
static Sint16 falling_water_frame_13_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_13_polygons.inc"
};
static Sint32 falling_water_frame_13_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_13_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_13_model = {
#include "stg34/scenery_art/falling_water_frame_13_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_13_object = {
#include "stg34/scenery_art/falling_water_frame_13_object.inc"
};
static Sint16 falling_water_frame_12_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_12_polygons.inc"
};
static Sint32 falling_water_frame_12_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_12_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_12_model = {
#include "stg34/scenery_art/falling_water_frame_12_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_12_object = {
#include "stg34/scenery_art/falling_water_frame_12_object.inc"
};
static Sint16 falling_water_frame_11_polygons[293] = {
#include "stg34/scenery_art/falling_water_frame_11_polygons.inc"
};
static Sint32 falling_water_frame_11_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_11_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_11_model = {
#include "stg34/scenery_art/falling_water_frame_11_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_11_object = {
#include "stg34/scenery_art/falling_water_frame_11_object.inc"
};
static Sint16 falling_water_frame_10_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_10_polygons.inc"
};
static Sint32 falling_water_frame_10_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_10_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_10_model = {
#include "stg34/scenery_art/falling_water_frame_10_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_10_object = {
#include "stg34/scenery_art/falling_water_frame_10_object.inc"
};
static Sint16 falling_water_frame_9_polygons[265] = {
#include "stg34/scenery_art/falling_water_frame_9_polygons.inc"
};
static Sint32 falling_water_frame_9_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_9_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_9_model = {
#include "stg34/scenery_art/falling_water_frame_9_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_9_object = {
#include "stg34/scenery_art/falling_water_frame_9_object.inc"
};
static Sint16 falling_water_frame_8_polygons[273] = {
#include "stg34/scenery_art/falling_water_frame_8_polygons.inc"
};
static Sint32 falling_water_frame_8_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_8_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_8_model = {
#include "stg34/scenery_art/falling_water_frame_8_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_8_object = {
#include "stg34/scenery_art/falling_water_frame_8_object.inc"
};
static Sint16 falling_water_frame_7_polygons[259] = {
#include "stg34/scenery_art/falling_water_frame_7_polygons.inc"
};
static Sint32 falling_water_frame_7_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_7_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_7_model = {
#include "stg34/scenery_art/falling_water_frame_7_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_7_object = {
#include "stg34/scenery_art/falling_water_frame_7_object.inc"
};
static Sint16 falling_water_frame_6_polygons[293] = {
#include "stg34/scenery_art/falling_water_frame_6_polygons.inc"
};
static Sint32 falling_water_frame_6_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_6_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_6_model = {
#include "stg34/scenery_art/falling_water_frame_6_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_6_object = {
#include "stg34/scenery_art/falling_water_frame_6_object.inc"
};
static Sint16 falling_water_frame_5_polygons[293] = {
#include "stg34/scenery_art/falling_water_frame_5_polygons.inc"
};
static Sint32 falling_water_frame_5_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_5_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_5_model = {
#include "stg34/scenery_art/falling_water_frame_5_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_5_object = {
#include "stg34/scenery_art/falling_water_frame_5_object.inc"
};
static Sint16 falling_water_frame_4_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_4_polygons.inc"
};
static Sint32 falling_water_frame_4_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_4_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_4_model = {
#include "stg34/scenery_art/falling_water_frame_4_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_4_object = {
#include "stg34/scenery_art/falling_water_frame_4_object.inc"
};
static Sint16 falling_water_frame_3_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_3_polygons.inc"
};
static Sint32 falling_water_frame_3_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_3_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_3_model = {
#include "stg34/scenery_art/falling_water_frame_3_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_3_object = {
#include "stg34/scenery_art/falling_water_frame_3_object.inc"
};
static Sint16 falling_water_frame_2_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_2_polygons.inc"
};
static Sint32 falling_water_frame_2_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_2_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_2_model = {
#include "stg34/scenery_art/falling_water_frame_2_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_2_object = {
#include "stg34/scenery_art/falling_water_frame_2_object.inc"
};
static Sint16 falling_water_frame_1_polygons[293] = {
#include "stg34/scenery_art/falling_water_frame_1_polygons.inc"
};
static Sint32 falling_water_frame_1_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_1_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_1_model = {
#include "stg34/scenery_art/falling_water_frame_1_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_1_object = {
#include "stg34/scenery_art/falling_water_frame_1_object.inc"
};
static Sint16 falling_water_frame_15_polygons[287] = {
#include "stg34/scenery_art/falling_water_frame_15_polygons.inc"
};
static Sint32 falling_water_frame_15_vertices[163] = {
#include "stg34/scenery_art/falling_water_frame_15_vertices.inc"
};
static NJS_CNK_MODEL falling_water_frame_15_model = {
#include "stg34/scenery_art/falling_water_frame_15_model.inc"
};
static NJS_CNK_OBJECT falling_water_frame_15_object = {
#include "stg34/scenery_art/falling_water_frame_15_object.inc"
};
static NJS_CNK_OBJECT falling_water_sequence_part_0 = {
#include "stg34/scenery_art/falling_water_sequence_part_0.inc"
};
static NJS_TEXNAME water_upper_archive_textures_names[1] = {
#include "stg34/scenery_art/water_upper_archive_textures_names.inc"
};
static NJS_TEXLIST water_upper_archive_textures = {
#include "stg34/scenery_art/water_upper_archive_textures.inc"
};
static Sint16 water_upper_part_5_polygons[161] = {
#include "stg34/scenery_art/water_upper_part_5_polygons.inc"
};
static Sint32 water_upper_part_5_vertices[135] = {
#include "stg34/scenery_art/water_upper_part_5_vertices.inc"
};
static NJS_CNK_MODEL water_upper_part_5_model = {
#include "stg34/scenery_art/water_upper_part_5_model.inc"
};
static NJS_CNK_OBJECT water_upper_part_5 = {
#include "stg34/scenery_art/water_upper_part_5.inc"
};
static Sint16 water_upper_part_4_polygons[89] = {
#include "stg34/scenery_art/water_upper_part_4_polygons.inc"
};
static Sint32 water_upper_part_4_vertices[63] = {
#include "stg34/scenery_art/water_upper_part_4_vertices.inc"
};
static NJS_CNK_MODEL water_upper_part_4_model = {
#include "stg34/scenery_art/water_upper_part_4_model.inc"
};
static NJS_CNK_OBJECT water_upper_part_4 = {
#include "stg34/scenery_art/water_upper_part_4.inc"
};
static Sint16 water_upper_part_3_polygons[107] = {
#include "stg34/scenery_art/water_upper_part_3_polygons.inc"
};
static Sint32 water_upper_part_3_vertices[75] = {
#include "stg34/scenery_art/water_upper_part_3_vertices.inc"
};
static NJS_CNK_MODEL water_upper_part_3_model = {
#include "stg34/scenery_art/water_upper_part_3_model.inc"
};
static NJS_CNK_OBJECT water_upper_part_3 = {
#include "stg34/scenery_art/water_upper_part_3.inc"
};
static Sint16 water_upper_part_2_polygons[89] = {
#include "stg34/scenery_art/water_upper_part_2_polygons.inc"
};
static Sint32 water_upper_part_2_vertices[63] = {
#include "stg34/scenery_art/water_upper_part_2_vertices.inc"
};
static NJS_CNK_MODEL water_upper_part_2_model = {
#include "stg34/scenery_art/water_upper_part_2_model.inc"
};
static NJS_CNK_OBJECT water_upper_part_2 = {
#include "stg34/scenery_art/water_upper_part_2.inc"
};
static Sint16 water_upper_part_1_polygons[107] = {
#include "stg34/scenery_art/water_upper_part_1_polygons.inc"
};
static Sint32 water_upper_part_1_vertices[75] = {
#include "stg34/scenery_art/water_upper_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_upper_part_1_model = {
#include "stg34/scenery_art/water_upper_part_1_model.inc"
};
static NJS_CNK_OBJECT water_upper_part_1 = {
#include "stg34/scenery_art/water_upper_part_1.inc"
};
NJS_CNK_OBJECT water_upper = {
#include "stg34/scenery_art/water_upper.inc"
};
static NJS_TEXNAME water_middle_archive_textures_names[1] = {
#include "stg34/scenery_art/water_middle_archive_textures_names.inc"
};
static NJS_TEXLIST water_middle_archive_textures = {
#include "stg34/scenery_art/water_middle_archive_textures.inc"
};
static Sint16 water_middle_part_13_polygons[95] = {
#include "stg34/scenery_art/water_middle_part_13_polygons.inc"
};
static Sint32 water_middle_part_13_vertices[63] = {
#include "stg34/scenery_art/water_middle_part_13_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_13_model = {
#include "stg34/scenery_art/water_middle_part_13_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_13 = {
#include "stg34/scenery_art/water_middle_part_13.inc"
};
static Sint16 water_middle_part_12_polygons[161] = {
#include "stg34/scenery_art/water_middle_part_12_polygons.inc"
};
static Sint32 water_middle_part_12_vertices[135] = {
#include "stg34/scenery_art/water_middle_part_12_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_12_model = {
#include "stg34/scenery_art/water_middle_part_12_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_12 = {
#include "stg34/scenery_art/water_middle_part_12.inc"
};
static Sint16 water_middle_part_11_polygons[107] = {
#include "stg34/scenery_art/water_middle_part_11_polygons.inc"
};
static Sint32 water_middle_part_11_vertices[75] = {
#include "stg34/scenery_art/water_middle_part_11_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_11_model = {
#include "stg34/scenery_art/water_middle_part_11_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_11 = {
#include "stg34/scenery_art/water_middle_part_11.inc"
};
static Sint16 water_middle_part_10_polygons[161] = {
#include "stg34/scenery_art/water_middle_part_10_polygons.inc"
};
static Sint32 water_middle_part_10_vertices[135] = {
#include "stg34/scenery_art/water_middle_part_10_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_10_model = {
#include "stg34/scenery_art/water_middle_part_10_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_10 = {
#include "stg34/scenery_art/water_middle_part_10.inc"
};
static Sint16 water_middle_part_9_polygons[107] = {
#include "stg34/scenery_art/water_middle_part_9_polygons.inc"
};
static Sint32 water_middle_part_9_vertices[75] = {
#include "stg34/scenery_art/water_middle_part_9_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_9_model = {
#include "stg34/scenery_art/water_middle_part_9_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_9 = {
#include "stg34/scenery_art/water_middle_part_9.inc"
};
static Sint16 water_middle_part_8_polygons[89] = {
#include "stg34/scenery_art/water_middle_part_8_polygons.inc"
};
static Sint32 water_middle_part_8_vertices[63] = {
#include "stg34/scenery_art/water_middle_part_8_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_8_model = {
#include "stg34/scenery_art/water_middle_part_8_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_8 = {
#include "stg34/scenery_art/water_middle_part_8.inc"
};
static Sint16 water_middle_part_7_polygons[89] = {
#include "stg34/scenery_art/water_middle_part_7_polygons.inc"
};
static Sint32 water_middle_part_7_vertices[63] = {
#include "stg34/scenery_art/water_middle_part_7_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_7_model = {
#include "stg34/scenery_art/water_middle_part_7_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_7 = {
#include "stg34/scenery_art/water_middle_part_7.inc"
};
static Sint16 water_middle_part_6_polygons[89] = {
#include "stg34/scenery_art/water_middle_part_6_polygons.inc"
};
static Sint32 water_middle_part_6_vertices[63] = {
#include "stg34/scenery_art/water_middle_part_6_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_6_model = {
#include "stg34/scenery_art/water_middle_part_6_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_6 = {
#include "stg34/scenery_art/water_middle_part_6.inc"
};
static Sint16 water_middle_part_5_polygons[161] = {
#include "stg34/scenery_art/water_middle_part_5_polygons.inc"
};
static Sint32 water_middle_part_5_vertices[135] = {
#include "stg34/scenery_art/water_middle_part_5_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_5_model = {
#include "stg34/scenery_art/water_middle_part_5_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_5 = {
#include "stg34/scenery_art/water_middle_part_5.inc"
};
static Sint16 water_middle_part_4_polygons[107] = {
#include "stg34/scenery_art/water_middle_part_4_polygons.inc"
};
static Sint32 water_middle_part_4_vertices[75] = {
#include "stg34/scenery_art/water_middle_part_4_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_4_model = {
#include "stg34/scenery_art/water_middle_part_4_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_4 = {
#include "stg34/scenery_art/water_middle_part_4.inc"
};
static Sint16 water_middle_part_3_polygons[161] = {
#include "stg34/scenery_art/water_middle_part_3_polygons.inc"
};
static Sint32 water_middle_part_3_vertices[135] = {
#include "stg34/scenery_art/water_middle_part_3_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_3_model = {
#include "stg34/scenery_art/water_middle_part_3_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_3 = {
#include "stg34/scenery_art/water_middle_part_3.inc"
};
static Sint16 water_middle_part_2_polygons[107] = {
#include "stg34/scenery_art/water_middle_part_2_polygons.inc"
};
static Sint32 water_middle_part_2_vertices[75] = {
#include "stg34/scenery_art/water_middle_part_2_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_2_model = {
#include "stg34/scenery_art/water_middle_part_2_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_2 = {
#include "stg34/scenery_art/water_middle_part_2.inc"
};
static Sint16 water_middle_part_1_polygons[161] = {
#include "stg34/scenery_art/water_middle_part_1_polygons.inc"
};
static Sint32 water_middle_part_1_vertices[135] = {
#include "stg34/scenery_art/water_middle_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_middle_part_1_model = {
#include "stg34/scenery_art/water_middle_part_1_model.inc"
};
static NJS_CNK_OBJECT water_middle_part_1 = {
#include "stg34/scenery_art/water_middle_part_1.inc"
};
NJS_CNK_OBJECT water_middle = {
#include "stg34/scenery_art/water_middle.inc"
};
static NJS_TEXNAME water_lower_archive_textures_names[1] = {
#include "stg34/scenery_art/water_lower_archive_textures_names.inc"
};
static NJS_TEXLIST water_lower_archive_textures = {
#include "stg34/scenery_art/water_lower_archive_textures.inc"
};
static Sint16 water_lower_part_8_polygons[107] = {
#include "stg34/scenery_art/water_lower_part_8_polygons.inc"
};
static Sint32 water_lower_part_8_vertices[75] = {
#include "stg34/scenery_art/water_lower_part_8_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_8_model = {
#include "stg34/scenery_art/water_lower_part_8_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_8 = {
#include "stg34/scenery_art/water_lower_part_8.inc"
};
static Sint16 water_lower_part_7_polygons[89] = {
#include "stg34/scenery_art/water_lower_part_7_polygons.inc"
};
static Sint32 water_lower_part_7_vertices[63] = {
#include "stg34/scenery_art/water_lower_part_7_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_7_model = {
#include "stg34/scenery_art/water_lower_part_7_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_7 = {
#include "stg34/scenery_art/water_lower_part_7.inc"
};
static Sint16 water_lower_part_6_polygons[89] = {
#include "stg34/scenery_art/water_lower_part_6_polygons.inc"
};
static Sint32 water_lower_part_6_vertices[63] = {
#include "stg34/scenery_art/water_lower_part_6_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_6_model = {
#include "stg34/scenery_art/water_lower_part_6_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_6 = {
#include "stg34/scenery_art/water_lower_part_6.inc"
};
static Sint16 water_lower_part_5_polygons[89] = {
#include "stg34/scenery_art/water_lower_part_5_polygons.inc"
};
static Sint32 water_lower_part_5_vertices[63] = {
#include "stg34/scenery_art/water_lower_part_5_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_5_model = {
#include "stg34/scenery_art/water_lower_part_5_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_5 = {
#include "stg34/scenery_art/water_lower_part_5.inc"
};
static Sint16 water_lower_part_4_polygons[161] = {
#include "stg34/scenery_art/water_lower_part_4_polygons.inc"
};
static Sint32 water_lower_part_4_vertices[135] = {
#include "stg34/scenery_art/water_lower_part_4_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_4_model = {
#include "stg34/scenery_art/water_lower_part_4_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_4 = {
#include "stg34/scenery_art/water_lower_part_4.inc"
};
static Sint16 water_lower_part_3_polygons[95] = {
#include "stg34/scenery_art/water_lower_part_3_polygons.inc"
};
static Sint32 water_lower_part_3_vertices[63] = {
#include "stg34/scenery_art/water_lower_part_3_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_3_model = {
#include "stg34/scenery_art/water_lower_part_3_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_3 = {
#include "stg34/scenery_art/water_lower_part_3.inc"
};
static Sint16 water_lower_part_2_polygons[95] = {
#include "stg34/scenery_art/water_lower_part_2_polygons.inc"
};
static Sint32 water_lower_part_2_vertices[63] = {
#include "stg34/scenery_art/water_lower_part_2_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_2_model = {
#include "stg34/scenery_art/water_lower_part_2_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_2 = {
#include "stg34/scenery_art/water_lower_part_2.inc"
};
static Sint16 water_lower_part_1_polygons[107] = {
#include "stg34/scenery_art/water_lower_part_1_polygons.inc"
};
static Sint32 water_lower_part_1_vertices[75] = {
#include "stg34/scenery_art/water_lower_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_lower_part_1_model = {
#include "stg34/scenery_art/water_lower_part_1_model.inc"
};
static NJS_CNK_OBJECT water_lower_part_1 = {
#include "stg34/scenery_art/water_lower_part_1.inc"
};
NJS_CNK_OBJECT water_lower = {
#include "stg34/scenery_art/water_lower.inc"
};
static NJS_TEXNAME water_end_archive_textures_names[1] = {
#include "stg34/scenery_art/water_end_archive_textures_names.inc"
};
static NJS_TEXLIST water_end_archive_textures = {
#include "stg34/scenery_art/water_end_archive_textures.inc"
};
static Sint16 water_end_part_1_polygons[501] = {
#include "stg34/scenery_art/water_end_part_1_polygons.inc"
};
static Sint32 water_end_part_1_vertices[351] = {
#include "stg34/scenery_art/water_end_part_1_vertices.inc"
};
static NJS_CNK_MODEL water_end_part_1_model = {
#include "stg34/scenery_art/water_end_part_1_model.inc"
};
static NJS_CNK_OBJECT water_end_part_1 = {
#include "stg34/scenery_art/water_end_part_1.inc"
};
NJS_CNK_OBJECT water_end = {
#include "stg34/scenery_art/water_end.inc"
};
static NJS_TEXNAME core_sonic_background_textures_names[10] = {
#include "stg34/scenery_art/core_sonic_background_textures_names.inc"
};
NJS_TEXLIST core_sonic_background_textures = {
#include "stg34/scenery_art/core_sonic_background_textures.inc"
};
NJS_TEXLIST *core_sonic_background_lists[23] = {
#include "stg34/scenery_art/core_sonic_background_lists.inc"
};
int core_rotor_turning = {
#include "stg34/scenery_art/core_rotor_turning.inc"
};
int core_rotor_tick = {
#include "stg34/scenery_art/core_rotor_tick.inc"
};
Angle core_rotor_angle = {
#include "stg34/scenery_art/core_rotor_angle.inc"
};
NJS_CNK_MODEL *inner_wall_frames[WALL_FRAMES] = {
#include "stg34/scenery_art/inner_wall_frames.inc"
};
NJS_CNK_MODEL *middle_wall_frames[WALL_FRAMES] = {
#include "stg34/scenery_art/middle_wall_frames.inc"
};
NJS_CNK_MODEL *outer_wall_frames[WALL_FRAMES] = {
#include "stg34/scenery_art/outer_wall_frames.inc"
};
NJS_CNK_MODEL *entrance_unit_frames[16] = {
#include "stg34/scenery_art/entrance_unit_frames.inc"
};
NJS_CNK_MODEL *entrance_flame_frames[49] = {
#include "stg34/scenery_art/entrance_flame_frames.inc"
};
NJS_CNK_MODEL *rotor_flame_frames[79] = {
#include "stg34/scenery_art/rotor_flame_frames.inc"
};
NJS_CNK_MODEL *rotor_wall_frames[8] = {
#include "stg34/scenery_art/rotor_wall_frames.inc"
};
NJS_CNK_MODEL *shaft_flame_frames[80] = {
#include "stg34/scenery_art/shaft_flame_frames.inc"
};
NJS_CNK_MODEL *shaft_unit_frames[16] = {
#include "stg34/scenery_art/shaft_unit_frames.inc"
};
NJS_CNK_MODEL *fan_wall_frames[8] = {
#include "stg34/scenery_art/fan_wall_frames.inc"
};
NJS_CNK_MODEL *pillar_frames[PILLAR_FRAMES] = {
#include "stg34/scenery_art/pillar_frames.inc"
};
NJS_CNK_MODEL *water_unit_frames[16] = {
#include "stg34/scenery_art/water_unit_frames.inc"
};
NJS_CNK_MODEL *upper_channel_frames[16] = {
#include "stg34/scenery_art/upper_channel_frames.inc"
};
NJS_CNK_MODEL *middle_channel_frames[16] = {
#include "stg34/scenery_art/middle_channel_frames.inc"
};
NJS_CNK_MODEL *lower_channel_frames[16] = {
#include "stg34/scenery_art/lower_channel_frames.inc"
};
NJS_CNK_MODEL *waterfall_frames[60] = {
#include "stg34/scenery_art/waterfall_frames.inc"
};
NJS_CNK_MODEL *falling_water_frames[30] = {
#include "stg34/scenery_art/falling_water_frames.inc"
};
