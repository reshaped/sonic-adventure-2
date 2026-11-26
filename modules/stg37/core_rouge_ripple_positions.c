/* STG37.PRS's texture lists, chunk models, chunk poses and UV scrolls, from
 * the disc. its D lies between objects.c and core_shaft_rotor.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "shared/core_scenery_art.h"
#include "shared/water_shaft/textures.h"
#include "stg37/core_rouge_water_filenames.h"
#include "stg37/water_cover.h"
#include "stg37/water_materials.h"
#include "stg37/water_poses.h"
#include "stg37/water_scenery.h"

struct rotor_water_poses_pose {
    Sint16 polygons[183];
    Sint32 vertices[123];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME entry_light_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/entry_light_textures_slots.inc"
};
NJS_TEXLIST entry_light_textures = {
#include "stg37/core_rouge_ripple_positions/entry_light_textures.inc"
};
static Sint16 entry_light_object_polygons[77] = {
#include "stg37/core_rouge_ripple_positions/entry_light_object_polygons.inc"
};
static Sint32 entry_light_object_vertices[83] = {
#include "stg37/core_rouge_ripple_positions/entry_light_object_vertices.inc"
};
NJS_CNK_MODEL entry_light = {
#include "stg37/core_rouge_ripple_positions/entry_light.inc"
};
static NJS_CNK_OBJECT entry_light_object = {
#include "stg37/core_rouge_ripple_positions/entry_light_object.inc"
};
static Sint16 cover_ripple_positions_a_panel_4_polygons[17] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_4_polygons.inc"
};
static Sint32 cover_ripple_positions_a_panel_4_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_4_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_a_panel_4_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_a_panel_4 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_4.inc"
};
static Sint16 cover_ripple_positions_a_panel_3_polygons[17] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_3_polygons.inc"
};
static Sint32 cover_ripple_positions_a_panel_3_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_3_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_a_panel_3_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_a_panel_3 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_3.inc"
};
static Sint16 cover_ripple_positions_a_panel_2_polygons[17] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_2_polygons.inc"
};
static Sint32 cover_ripple_positions_a_panel_2_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_2_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_a_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_a_panel_2 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_2.inc"
};
static Sint16 cover_ripple_positions_a_panel_1_polygons[17] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_1_polygons.inc"
};
static Sint32 cover_ripple_positions_a_panel_1_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_1_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_a_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_a_panel_1 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a_panel_1.inc"
};
NJS_CNK_OBJECT cover_ripple_positions_a = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_a.inc"
};
static NJS_TEXNAME second_ripple_marker_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/second_ripple_marker_textures_slots.inc"
};
static NJS_TEXLIST second_ripple_marker_textures = {
#include "stg37/core_rouge_ripple_positions/second_ripple_marker_textures.inc"
};
static Sint16 cover_ripple_positions_b_panel_4_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_4_polygons.inc"
};
static Sint32 cover_ripple_positions_b_panel_4_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_4_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_b_panel_4_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_b_panel_4 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_4.inc"
};
static Sint16 cover_ripple_positions_b_panel_3_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_3_polygons.inc"
};
static Sint32 cover_ripple_positions_b_panel_3_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_3_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_b_panel_3_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_b_panel_3 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_3.inc"
};
static Sint16 cover_ripple_positions_b_panel_2_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_2_polygons.inc"
};
static Sint32 cover_ripple_positions_b_panel_2_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_2_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_b_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_b_panel_2 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_2.inc"
};
static Sint16 cover_ripple_positions_b_panel_1_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_1_polygons.inc"
};
static Sint32 cover_ripple_positions_b_panel_1_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_1_vertices.inc"
};
static NJS_CNK_MODEL cover_ripple_positions_b_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT cover_ripple_positions_b_panel_1 = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b_panel_1.inc"
};
NJS_CNK_OBJECT cover_ripple_positions_b = {
#include "stg37/core_rouge_ripple_positions/cover_ripple_positions_b.inc"
};
static NJS_TEXNAME column_textures_slot[1] = {
#include "stg37/core_rouge_ripple_positions/column_textures_slot.inc"
};
NJS_TEXLIST column_textures = {
#include "stg37/core_rouge_ripple_positions/column_textures.inc"
};
static Sint16 rising_column_panel_12_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_12_polygons.inc"
};
static Sint32 rising_column_panel_12_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_12_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_12_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_12_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_12 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_12.inc"
};
static Sint16 rising_column_panel_11_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_11_polygons.inc"
};
static Sint32 rising_column_panel_11_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_11_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_11_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_11_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_11 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_11.inc"
};
static Sint16 rising_column_panel_10_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_10_polygons.inc"
};
static Sint32 rising_column_panel_10_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_10_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_10_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_10_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_10 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_10.inc"
};
static Sint16 rising_column_panel_9_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_9_polygons.inc"
};
static Sint32 rising_column_panel_9_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_9_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_9_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_9_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_9 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_9.inc"
};
static Sint16 rising_column_panel_8_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_8_polygons.inc"
};
static Sint32 rising_column_panel_8_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_8_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_8_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_8_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_8 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_8.inc"
};
static Sint16 rising_column_panel_7_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_7_polygons.inc"
};
static Sint32 rising_column_panel_7_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_7_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_7_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_7_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_7 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_7.inc"
};
static Sint16 rising_column_panel_6_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_6_polygons.inc"
};
static Sint32 rising_column_panel_6_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_6_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_6_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_6 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_6.inc"
};
static Sint16 rising_column_panel_5_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_5_polygons.inc"
};
static Sint32 rising_column_panel_5_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_5_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_5_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_5 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_5.inc"
};
static Sint16 rising_column_panel_4_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_4_polygons.inc"
};
static Sint32 rising_column_panel_4_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_4_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_4_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_4 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_4.inc"
};
static Sint16 rising_column_panel_3_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_3_polygons.inc"
};
static Sint32 rising_column_panel_3_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_3_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_3_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_3 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_3.inc"
};
static Sint16 rising_column_panel_2_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_2_polygons.inc"
};
static Sint32 rising_column_panel_2_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_2_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_2 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_2.inc"
};
static Sint16 rising_column_panel_1_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_1_polygons.inc"
};
static Sint32 rising_column_panel_1_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_1_vertices.inc"
};
static NJS_CNK_MODEL rising_column_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT rising_column_panel_1 = {
#include "stg37/core_rouge_ripple_positions/rising_column_panel_1.inc"
};
NJS_CNK_OBJECT rising_columns = {
#include "stg37/core_rouge_ripple_positions/rising_columns.inc"
};
static NJS_TEXNAME falling_columns_textures_slot[1] = {
#include "stg37/core_rouge_ripple_positions/falling_columns_textures_slot.inc"
};
NJS_TEXLIST falling_columns_textures = {
#include "stg37/core_rouge_ripple_positions/falling_columns_textures.inc"
};
static Sint16 falling_column_panel_12_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_12_polygons.inc"
};
static Sint32 falling_column_panel_12_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_12_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_12_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_12_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_12 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_12.inc"
};
static Sint16 falling_column_panel_11_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_11_polygons.inc"
};
static Sint32 falling_column_panel_11_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_11_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_11_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_11_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_11 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_11.inc"
};
static Sint16 falling_column_panel_10_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_10_polygons.inc"
};
static Sint32 falling_column_panel_10_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_10_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_10_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_10_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_10 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_10.inc"
};
static Sint16 falling_column_panel_9_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_9_polygons.inc"
};
static Sint32 falling_column_panel_9_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_9_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_9_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_9_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_9 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_9.inc"
};
static Sint16 falling_column_panel_8_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_8_polygons.inc"
};
static Sint32 falling_column_panel_8_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_8_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_8_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_8_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_8 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_8.inc"
};
static Sint16 falling_column_panel_7_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_7_polygons.inc"
};
static Sint32 falling_column_panel_7_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_7_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_7_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_7_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_7 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_7.inc"
};
static Sint16 falling_column_panel_6_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_6_polygons.inc"
};
static Sint32 falling_column_panel_6_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_6_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_6_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_6_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_6 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_6.inc"
};
static Sint16 falling_column_panel_5_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_5_polygons.inc"
};
static Sint32 falling_column_panel_5_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_5_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_5_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_5_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_5 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_5.inc"
};
static Sint16 falling_column_panel_4_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_4_polygons.inc"
};
static Sint32 falling_column_panel_4_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_4_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_4_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_4_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_4 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_4.inc"
};
static Sint16 falling_column_panel_3_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_3_polygons.inc"
};
static Sint32 falling_column_panel_3_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_3_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_3_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_3 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_3.inc"
};
static Sint16 falling_column_panel_2_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_2_polygons.inc"
};
static Sint32 falling_column_panel_2_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_2_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_2 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_2.inc"
};
static Sint16 falling_column_panel_1_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_1_polygons.inc"
};
static Sint32 falling_column_panel_1_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_1_vertices.inc"
};
static NJS_CNK_MODEL falling_column_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT falling_column_panel_1 = {
#include "stg37/core_rouge_ripple_positions/falling_column_panel_1.inc"
};
NJS_CNK_OBJECT falling_columns = {
#include "stg37/core_rouge_ripple_positions/falling_columns.inc"
};
static NJS_TEXNAME cover_panel_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_textures_slots.inc"
};
NJS_TEXLIST cover_panel_textures = {
#include "stg37/core_rouge_ripple_positions/cover_panel_textures.inc"
};
static Sint16 cover_panel_position_panel_3_polygons[63] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_3_polygons.inc"
};
static Sint32 cover_panel_position_panel_3_vertices[67] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_3_vertices.inc"
};
static NJS_CNK_MODEL cover_panel_position_panel_3_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_3_mesh.inc"
};
static NJS_CNK_OBJECT cover_panel_position_panel_3 = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_3.inc"
};
static Sint16 cover_panel_position_panel_2_polygons[63] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_2_polygons.inc"
};
static Sint32 cover_panel_position_panel_2_vertices[67] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_2_vertices.inc"
};
static NJS_CNK_MODEL cover_panel_position_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT cover_panel_position_panel_2 = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_2.inc"
};
static Sint16 cover_panel_position_panel_1_polygons[63] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_1_polygons.inc"
};
static Sint32 cover_panel_position_panel_1_vertices[67] = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_1_vertices.inc"
};
static NJS_CNK_MODEL cover_panel_position_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT cover_panel_position_panel_1 = {
#include "stg37/core_rouge_ripple_positions/cover_panel_position_panel_1.inc"
};
NJS_CNK_OBJECT cover_panel_positions = {
#include "stg37/core_rouge_ripple_positions/cover_panel_positions.inc"
};
static NJS_TEXNAME shaft_textures_slot[1] = {
#include "stg37/core_rouge_ripple_positions/shaft_textures_slot.inc"
};
NJS_TEXLIST shaft_textures = {
#include "stg37/core_rouge_ripple_positions/shaft_textures.inc"
};
static Sint16 shaft_entry_wall_panel_2_polygons[273] = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_2_polygons.inc"
};
static Sint32 shaft_entry_wall_panel_2_vertices[123] = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_entry_wall_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT shaft_entry_wall_panel_2 = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_2.inc"
};
static Sint16 shaft_entry_wall_panel_1_polygons[273] = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_1_polygons.inc"
};
static Sint32 shaft_entry_wall_panel_1_vertices[123] = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_entry_wall_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT shaft_entry_wall_panel_1 = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_wall_panel_1.inc"
};
NJS_CNK_OBJECT shaft_entry_walls = {
#include "stg37/core_rouge_ripple_positions/shaft_entry_walls.inc"
};
static NJS_TEXNAME shaft_walls_textures_slot[1] = {
#include "stg37/core_rouge_ripple_positions/shaft_walls_textures_slot.inc"
};
NJS_TEXLIST shaft_walls_textures = {
#include "stg37/core_rouge_ripple_positions/shaft_walls_textures.inc"
};
static Sint16 shaft_wall_panel_2_polygons[273] = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_2_polygons.inc"
};
static Sint32 shaft_wall_panel_2_vertices[123] = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_2_vertices.inc"
};
static NJS_CNK_MODEL shaft_wall_panel_2_mesh = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_2_mesh.inc"
};
static NJS_CNK_OBJECT shaft_wall_panel_2 = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_2.inc"
};
static Sint16 shaft_wall_panel_1_polygons[273] = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_1_polygons.inc"
};
static Sint32 shaft_wall_panel_1_vertices[123] = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_1_vertices.inc"
};
static NJS_CNK_MODEL shaft_wall_panel_1_mesh = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_1_mesh.inc"
};
static NJS_CNK_OBJECT shaft_wall_panel_1 = {
#include "stg37/core_rouge_ripple_positions/shaft_wall_panel_1.inc"
};
NJS_CNK_OBJECT shaft_walls = {
#include "stg37/core_rouge_ripple_positions/shaft_walls.inc"
};
static NJS_TEXNAME left_surface_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/left_surface_textures_slots.inc"
};
static NJS_TEXLIST left_surface_textures = {
#include "stg37/core_rouge_ripple_positions/left_surface_textures.inc"
};
static Sint16 left_surface_polygons[111] = {
#include "stg37/core_rouge_ripple_positions/left_surface_polygons.inc"
};
static Sint32 left_surface_vertices[131] = {
#include "stg37/core_rouge_ripple_positions/left_surface_vertices.inc"
};
static NJS_CNK_MODEL left_surface_mesh = {
#include "stg37/core_rouge_ripple_positions/left_surface_mesh.inc"
};
NJS_CNK_OBJECT left_surface = {
#include "stg37/core_rouge_ripple_positions/left_surface.inc"
};
static NJS_TEXNAME right_surface_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/right_surface_textures_slots.inc"
};
static NJS_TEXLIST right_surface_textures = {
#include "stg37/core_rouge_ripple_positions/right_surface_textures.inc"
};
static Sint16 right_surface_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/right_surface_polygons.inc"
};
static Sint32 right_surface_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/right_surface_vertices.inc"
};
static NJS_CNK_MODEL right_surface_mesh = {
#include "stg37/core_rouge_ripple_positions/right_surface_mesh.inc"
};
NJS_CNK_OBJECT right_surface = {
#include "stg37/core_rouge_ripple_positions/right_surface.inc"
};
static NJS_TEXNAME surface_pose_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_textures_slots.inc"
};
static NJS_TEXLIST surface_pose_textures = {
#include "stg37/core_rouge_ripple_positions/surface_pose_textures.inc"
};
static Sint16 surface_pose_31_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_31_polygons.inc"
};
static Sint32 surface_pose_31_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_31_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_31_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_31_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_31 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_31.inc"
};
static Sint16 surface_pose_30_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_30_polygons.inc"
};
static Sint32 surface_pose_30_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_30_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_30_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_30_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_30 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_30.inc"
};
static Sint16 surface_pose_29_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_29_polygons.inc"
};
static Sint32 surface_pose_29_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_29_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_29_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_29_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_29 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_29.inc"
};
static Sint16 surface_pose_28_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_28_polygons.inc"
};
static Sint32 surface_pose_28_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_28_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_28_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_28_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_28 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_28.inc"
};
static Sint16 surface_pose_27_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_27_polygons.inc"
};
static Sint32 surface_pose_27_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_27_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_27_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_27_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_27 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_27.inc"
};
static Sint16 surface_pose_26_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_26_polygons.inc"
};
static Sint32 surface_pose_26_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_26_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_26_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_26_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_26 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_26.inc"
};
static Sint16 surface_pose_25_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_25_polygons.inc"
};
static Sint32 surface_pose_25_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_25_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_25_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_25_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_25 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_25.inc"
};
static Sint16 surface_pose_24_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_24_polygons.inc"
};
static Sint32 surface_pose_24_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_24_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_24_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_24_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_24 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_24.inc"
};
static Sint16 surface_pose_23_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_23_polygons.inc"
};
static Sint32 surface_pose_23_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_23_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_23_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_23_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_23 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_23.inc"
};
static Sint16 surface_pose_22_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_22_polygons.inc"
};
static Sint32 surface_pose_22_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_22_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_22_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_22_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_22 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_22.inc"
};
static Sint16 surface_pose_21_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_21_polygons.inc"
};
static Sint32 surface_pose_21_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_21_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_21_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_21_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_21 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_21.inc"
};
static Sint16 surface_pose_20_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_20_polygons.inc"
};
static Sint32 surface_pose_20_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_20_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_20_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_20_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_20 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_20.inc"
};
static Sint16 surface_pose_19_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_19_polygons.inc"
};
static Sint32 surface_pose_19_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_19_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_19_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_19_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_19 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_19.inc"
};
static Sint16 surface_pose_18_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_18_polygons.inc"
};
static Sint32 surface_pose_18_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_18_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_18_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_18_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_18 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_18.inc"
};
static Sint16 surface_pose_17_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_17_polygons.inc"
};
static Sint32 surface_pose_17_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_17_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_17_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_17_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_17 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_17.inc"
};
static Sint16 surface_pose_16_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_16_polygons.inc"
};
static Sint32 surface_pose_16_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_16_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_16_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_16_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_16 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_16.inc"
};
static Sint16 surface_pose_15_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_15_polygons.inc"
};
static Sint32 surface_pose_15_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_15_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_15_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_15_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_15 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_15.inc"
};
static Sint16 surface_pose_14_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_14_polygons.inc"
};
static Sint32 surface_pose_14_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_14_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_14_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_14_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_14 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_14.inc"
};
static Sint16 surface_pose_13_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_13_polygons.inc"
};
static Sint32 surface_pose_13_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_13_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_13_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_13_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_13 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_13.inc"
};
static Sint16 surface_pose_12_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_12_polygons.inc"
};
static Sint32 surface_pose_12_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_12_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_12_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_12_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_12 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_12.inc"
};
static Sint16 surface_pose_11_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_11_polygons.inc"
};
static Sint32 surface_pose_11_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_11_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_11_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_11_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_11 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_11.inc"
};
static Sint16 surface_pose_10_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_10_polygons.inc"
};
static Sint32 surface_pose_10_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_10_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_10_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_10_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_10 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_10.inc"
};
static Sint16 surface_pose_9_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_9_polygons.inc"
};
static Sint32 surface_pose_9_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_9_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_9_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_9_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_9 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_9.inc"
};
static Sint16 surface_pose_8_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_8_polygons.inc"
};
static Sint32 surface_pose_8_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_8_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_8_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_8_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_8 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_8.inc"
};
static Sint16 surface_pose_7_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_7_polygons.inc"
};
static Sint32 surface_pose_7_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_7_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_7_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_7_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_7 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_7.inc"
};
static Sint16 surface_pose_6_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_6_polygons.inc"
};
static Sint32 surface_pose_6_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_6_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_6_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_6_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_6 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_6.inc"
};
static Sint16 surface_pose_5_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_5_polygons.inc"
};
static Sint32 surface_pose_5_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_5_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_5_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_5_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_5 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_5.inc"
};
static Sint16 surface_pose_4_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_4_polygons.inc"
};
static Sint32 surface_pose_4_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_4_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_4_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_4_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_4 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_4.inc"
};
static Sint16 surface_pose_3_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_3_polygons.inc"
};
static Sint32 surface_pose_3_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_3_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_3_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_3_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_3 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_3.inc"
};
static Sint16 surface_pose_2_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_2_polygons.inc"
};
static Sint32 surface_pose_2_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_2_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_2_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_2_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_2 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_2.inc"
};
static Sint16 surface_pose_1_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_1_polygons.inc"
};
static Sint32 surface_pose_1_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_1_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_1_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_1_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_1 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_1.inc"
};
static Sint16 surface_pose_0_polygons[559] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_0_polygons.inc"
};
static Sint32 surface_pose_0_vertices[355] = {
#include "stg37/core_rouge_ripple_positions/surface_pose_0_vertices.inc"
};
static NJS_CNK_MODEL surface_pose_0_mesh = {
#include "stg37/core_rouge_ripple_positions/surface_pose_0_mesh.inc"
};
NJS_CNK_OBJECT surface_pose_0 = {
#include "stg37/core_rouge_ripple_positions/surface_pose_0.inc"
};
static NJS_CNK_OBJECT surface_sequence = {
#include "stg37/core_rouge_ripple_positions/surface_sequence.inc"
};
static NJS_TEXNAME rotor_water_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/rotor_water_textures_slots.inc"
};
NJS_TEXLIST rotor_water_textures = {
#include "stg37/core_rouge_ripple_positions/rotor_water_textures.inc"
};
struct rotor_water_pose rotor_water_poses[20] = {
#include "stg37/core_rouge_ripple_positions/rotor_water_poses.inc"
};
static NJS_CNK_OBJECT rotor_water_sequence = {
#include "stg37/core_rouge_ripple_positions/rotor_water_sequence.inc"
};
static Sint16 moving_water_uvs[68] = {
#include "stg37/core_rouge_ripple_positions/moving_water_uvs.inc"
};
struct texture_anim moving_water_uv = {
#include "stg37/core_rouge_ripple_positions/moving_water_uv.inc"
};
static NJS_TEXNAME moving_water_sheet_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/moving_water_sheet_textures_slots.inc"
};
static NJS_TEXLIST moving_water_sheet_textures = {
#include "stg37/core_rouge_ripple_positions/moving_water_sheet_textures.inc"
};
static Sint16 moving_water_polygons[105] = {
#include "stg37/core_rouge_ripple_positions/moving_water_polygons.inc"
};
static Sint32 moving_water_vertices[87] = {
#include "stg37/core_rouge_ripple_positions/moving_water_vertices.inc"
};
NJS_CNK_MODEL moving_water_model = {
#include "stg37/core_rouge_ripple_positions/moving_water_model.inc"
};
NJS_CNK_OBJECT moving_water = {
#include "stg37/core_rouge_ripple_positions/moving_water.inc"
};
static NJS_TEXNAME lower_surface_textures_slots[1] = {
#include "stg37/core_rouge_ripple_positions/lower_surface_textures_slots.inc"
};
static NJS_TEXLIST lower_surface_textures = {
#include "stg37/core_rouge_ripple_positions/lower_surface_textures.inc"
};
static Sint16 lower_surface_polygons[31] = {
#include "stg37/core_rouge_ripple_positions/lower_surface_polygons.inc"
};
static Sint32 lower_surface_vertices[27] = {
#include "stg37/core_rouge_ripple_positions/lower_surface_vertices.inc"
};
static NJS_CNK_MODEL lower_surface_mesh = {
#include "stg37/core_rouge_ripple_positions/lower_surface_mesh.inc"
};
NJS_CNK_OBJECT lower_surface = {
#include "stg37/core_rouge_ripple_positions/lower_surface.inc"
};
