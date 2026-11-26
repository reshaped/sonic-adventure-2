#include <ninja.h>
#include <sg_xpt.h>

#include "cart/course_art.h"
#include "cart/kart_art.h"
#include "cart/objects.h"
#include "cart/ramp_art.h"
#include "cart/texture_remap.h"
#include "cart/track.h"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char cart_course_texture_filenames[2365] = {
#include "cart/course_art/cart_course_texture_filenames.inc"
};

/* CART.PRS's chunk models, Basic models, texture lists, course pieces and
 * texture remaps, from the disc. its D lies between race_setup.c and
 * track_draw.c */
static Sint16 cart_jump_model_polygons[177] = {
#include "cart/course_art/cart_jump_model_polygons.inc"
};
static Sint32 cart_jump_model_vertices[139] = {
#include "cart/course_art/cart_jump_model_vertices.inc"
};
static NJS_CNK_MODEL cart_jump_model_model = {
#include "cart/course_art/cart_jump_model_model.inc"
};
NJS_CNK_OBJECT cart_jump_model = {
#include "cart/course_art/cart_jump_model.inc"
};
static Sint16 cart_checkpoint_model_polygons[881] = {
#include "cart/course_art/cart_checkpoint_model_polygons.inc"
};
static Sint32 cart_checkpoint_model_vertices[979] = {
#include "cart/course_art/cart_checkpoint_model_vertices.inc"
};
static NJS_CNK_MODEL cart_checkpoint_model_model = {
#include "cart/course_art/cart_checkpoint_model_model.inc"
};
NJS_CNK_OBJECT cart_checkpoint_model[1] = {
#include "cart/course_art/cart_checkpoint_model.inc"
};
static Sint16 cart_dash_frame_polygons[107] = {
#include "cart/course_art/cart_dash_frame_polygons.inc"
};
static Sint32 cart_dash_frame_vertices[83] = {
#include "cart/course_art/cart_dash_frame_vertices.inc"
};
static NJS_CNK_MODEL cart_dash_frame_model = {
#include "cart/course_art/cart_dash_frame_model.inc"
};
NJS_CNK_OBJECT cart_dash_frame = {
#include "cart/course_art/cart_dash_frame.inc"
};
static Sint16 cart_dash_belt_polygons[113] = {
#include "cart/course_art/cart_dash_belt_polygons.inc"
};
static Sint32 cart_dash_belt_vertices[83] = {
#include "cart/course_art/cart_dash_belt_vertices.inc"
};
static NJS_CNK_MODEL cart_dash_belt_model = {
#include "cart/course_art/cart_dash_belt_model.inc"
};
NJS_CNK_OBJECT cart_dash_belt = {
#include "cart/course_art/cart_dash_belt.inc"
};
static Sint16 cart_race_lamp_polygons[25] = {
#include "cart/course_art/cart_race_lamp_polygons.inc"
};
static Sint32 cart_race_lamp_vertices[19] = {
#include "cart/course_art/cart_race_lamp_vertices.inc"
};
static NJS_CNK_MODEL cart_race_lamp_model = {
#include "cart/course_art/cart_race_lamp_model.inc"
};
NJS_CNK_OBJECT cart_race_lamp[1] = {
#include "cart/course_art/cart_race_lamp.inc"
};
static Sint16 cart_countdown_lamp_next_polygons[57] = {
#include "cart/course_art/cart_countdown_lamp_next_polygons.inc"
};
static Sint32 cart_countdown_lamp_next_vertices[35] = {
#include "cart/course_art/cart_countdown_lamp_next_vertices.inc"
};
static NJS_CNK_MODEL cart_countdown_lamp_next_model = {
#include "cart/course_art/cart_countdown_lamp_next_model.inc"
};
static NJS_CNK_OBJECT cart_countdown_lamp_next = {
#include "cart/course_art/cart_countdown_lamp_next.inc"
};
static Sint16 cart_countdown_lamp_polygons[25] = {
#include "cart/course_art/cart_countdown_lamp_polygons.inc"
};
static Sint32 cart_countdown_lamp_vertices[19] = {
#include "cart/course_art/cart_countdown_lamp_vertices.inc"
};
static NJS_CNK_MODEL cart_countdown_lamp_model = {
#include "cart/course_art/cart_countdown_lamp_model.inc"
};
NJS_CNK_OBJECT cart_countdown_lamp[1] = {
#include "cart/course_art/cart_countdown_lamp.inc"
};
static Sint16 cart_gate_model_child_polygons[57] = {
#include "cart/course_art/cart_gate_model_child_polygons.inc"
};
static Sint32 cart_gate_model_child_vertices[35] = {
#include "cart/course_art/cart_gate_model_child_vertices.inc"
};
static NJS_CNK_MODEL cart_gate_model_child_model = {
#include "cart/course_art/cart_gate_model_child_model.inc"
};
static NJS_CNK_OBJECT cart_gate_model_child = {
#include "cart/course_art/cart_gate_model_child.inc"
};
static Sint16 cart_gate_model_polygons[907] = {
#include "cart/course_art/cart_gate_model_polygons.inc"
};
static Sint32 cart_gate_model_vertices[899] = {
#include "cart/course_art/cart_gate_model_vertices.inc"
};
static NJS_CNK_MODEL cart_gate_model_model = {
#include "cart/course_art/cart_gate_model_model.inc"
};
NJS_CNK_OBJECT cart_gate_model[1] = {
#include "cart/course_art/cart_gate_model.inc"
};
static Sint16 cart_final_lamp_polygons[25] = {
#include "cart/course_art/cart_final_lamp_polygons.inc"
};
static Sint32 cart_final_lamp_vertices[19] = {
#include "cart/course_art/cart_final_lamp_vertices.inc"
};
static NJS_CNK_MODEL cart_final_lamp_model = {
#include "cart/course_art/cart_final_lamp_model.inc"
};
NJS_CNK_OBJECT cart_final_lamp[1] = {
#include "cart/course_art/cart_final_lamp.inc"
};
static Sint16 cart_sign_model_child_polygons[25] = {
#include "cart/course_art/cart_sign_model_child_polygons.inc"
};
static Sint32 cart_sign_model_child_vertices[19] = {
#include "cart/course_art/cart_sign_model_child_vertices.inc"
};
static NJS_CNK_MODEL cart_sign_model_child_model = {
#include "cart/course_art/cart_sign_model_child_model.inc"
};
static NJS_CNK_OBJECT cart_sign_model_child = {
#include "cart/course_art/cart_sign_model_child.inc"
};
static Sint16 cart_sign_model_polygons[247] = {
#include "cart/course_art/cart_sign_model_polygons.inc"
};
static Sint32 cart_sign_model_vertices[239] = {
#include "cart/course_art/cart_sign_model_vertices.inc"
};
static NJS_CNK_MODEL cart_sign_model_model = {
#include "cart/course_art/cart_sign_model_model.inc"
};
NJS_CNK_OBJECT cart_sign_model = {
#include "cart/course_art/cart_sign_model.inc"
};
static Sint16 cart_sign_first_polygons[875] = {
#include "cart/course_art/cart_sign_first_polygons.inc"
};
static Sint32 cart_sign_first_vertices[895] = {
#include "cart/course_art/cart_sign_first_vertices.inc"
};
static NJS_CNK_MODEL cart_sign_first_model = {
#include "cart/course_art/cart_sign_first_model.inc"
};
NJS_CNK_OBJECT cart_sign_first = {
#include "cart/course_art/cart_sign_first.inc"
};
static Sint16 cart_sign_panel_polygons[171] = {
#include "cart/course_art/cart_sign_panel_polygons.inc"
};
static Sint32 cart_sign_panel_vertices[195] = {
#include "cart/course_art/cart_sign_panel_vertices.inc"
};
static NJS_CNK_MODEL cart_sign_panel_model = {
#include "cart/course_art/cart_sign_panel_model.inc"
};
NJS_CNK_OBJECT cart_sign_panel = {
#include "cart/course_art/cart_sign_panel.inc"
};
static Sint16 cart_sign_second_polygons[871] = {
#include "cart/course_art/cart_sign_second_polygons.inc"
};
static Sint32 cart_sign_second_vertices[895] = {
#include "cart/course_art/cart_sign_second_vertices.inc"
};
static NJS_CNK_MODEL cart_sign_second_model = {
#include "cart/course_art/cart_sign_second_model.inc"
};
NJS_CNK_OBJECT cart_sign_second = {
#include "cart/course_art/cart_sign_second.inc"
};
static Sint16 cart_airship_first_polygons[779] = {
#include "cart/course_art/cart_airship_first_polygons.inc"
};
static Sint32 cart_airship_first_vertices[375] = {
#include "cart/course_art/cart_airship_first_vertices.inc"
};
static NJS_CNK_MODEL cart_airship_first_model = {
#include "cart/course_art/cart_airship_first_model.inc"
};
NJS_CNK_OBJECT cart_airship_first = {
#include "cart/course_art/cart_airship_first.inc"
};
static Sint16 cart_airship_second_polygons[779] = {
#include "cart/course_art/cart_airship_second_polygons.inc"
};
static Sint32 cart_airship_second_vertices[375] = {
#include "cart/course_art/cart_airship_second_vertices.inc"
};
static NJS_CNK_MODEL cart_airship_second_model = {
#include "cart/course_art/cart_airship_second_model.inc"
};
NJS_CNK_OBJECT cart_airship_second = {
#include "cart/course_art/cart_airship_second.inc"
};
static Sint16 cart_kakusi_model_polygons[31] = {
#include "cart/course_art/cart_kakusi_model_polygons.inc"
};
static Sint32 cart_kakusi_model_vertices[27] = {
#include "cart/course_art/cart_kakusi_model_vertices.inc"
};
static NJS_CNK_MODEL cart_kakusi_model_model = {
#include "cart/course_art/cart_kakusi_model_model.inc"
};
NJS_CNK_OBJECT cart_kakusi_model = {
#include "cart/course_art/cart_kakusi_model.inc"
};
static Sint16 cart_player_marker_1_polygons[25] = {
#include "cart/course_art/cart_player_marker_1_polygons.inc"
};
static Sint32 cart_player_marker_1_vertices[27] = {
#include "cart/course_art/cart_player_marker_1_vertices.inc"
};
static NJS_CNK_MODEL cart_player_marker_1_model = {
#include "cart/course_art/cart_player_marker_1_model.inc"
};
NJS_CNK_OBJECT cart_player_marker_1 = {
#include "cart/course_art/cart_player_marker_1.inc"
};
static Sint16 cart_player_marker_2_polygons[25] = {
#include "cart/course_art/cart_player_marker_2_polygons.inc"
};
static Sint32 cart_player_marker_2_vertices[27] = {
#include "cart/course_art/cart_player_marker_2_vertices.inc"
};
static NJS_CNK_MODEL cart_player_marker_2_model = {
#include "cart/course_art/cart_player_marker_2_model.inc"
};
NJS_CNK_OBJECT cart_player_marker_2 = {
#include "cart/course_art/cart_player_marker_2.inc"
};
static Sint16 piece_01_draw_polygons[1977] = {
#include "cart/course_art/piece_01_draw_polygons.inc"
};
static Sint32 piece_01_draw_vertices[1787] = {
#include "cart/course_art/piece_01_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_01_draw_model = {
#include "cart/course_art/piece_01_draw_model.inc"
};
static NJS_CNK_OBJECT piece_01_draw = {
#include "cart/course_art/piece_01_draw.inc"
};
static Sint16 piece_02_draw_polygons[2175] = {
#include "cart/course_art/piece_02_draw_polygons.inc"
};
static Sint32 piece_02_draw_vertices[1859] = {
#include "cart/course_art/piece_02_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_02_draw_model = {
#include "cart/course_art/piece_02_draw_model.inc"
};
static NJS_CNK_OBJECT piece_02_draw = {
#include "cart/course_art/piece_02_draw.inc"
};
static Sint16 piece_03_draw_polygons[2063] = {
#include "cart/course_art/piece_03_draw_polygons.inc"
};
static Sint32 piece_03_draw_vertices[1787] = {
#include "cart/course_art/piece_03_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_03_draw_model = {
#include "cart/course_art/piece_03_draw_model.inc"
};
static NJS_CNK_OBJECT piece_03_draw = {
#include "cart/course_art/piece_03_draw.inc"
};
static Sint16 piece_04_draw_polygons[1955] = {
#include "cart/course_art/piece_04_draw_polygons.inc"
};
static Sint32 piece_04_draw_vertices[1747] = {
#include "cart/course_art/piece_04_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_04_draw_model = {
#include "cart/course_art/piece_04_draw_model.inc"
};
static NJS_CNK_OBJECT piece_04_draw = {
#include "cart/course_art/piece_04_draw.inc"
};
static Sint16 piece_05_draw_polygons[2159] = {
#include "cart/course_art/piece_05_draw_polygons.inc"
};
static Sint32 piece_05_draw_vertices[1843] = {
#include "cart/course_art/piece_05_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_05_draw_model = {
#include "cart/course_art/piece_05_draw_model.inc"
};
static NJS_CNK_OBJECT piece_05_draw = {
#include "cart/course_art/piece_05_draw.inc"
};
static Sint16 piece_06_draw_polygons[2155] = {
#include "cart/course_art/piece_06_draw_polygons.inc"
};
static Sint32 piece_06_draw_vertices[1847] = {
#include "cart/course_art/piece_06_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_06_draw_model = {
#include "cart/course_art/piece_06_draw_model.inc"
};
static NJS_CNK_OBJECT piece_06_draw = {
#include "cart/course_art/piece_06_draw.inc"
};
static Sint16 piece_07_draw_polygons[4339] = {
#include "cart/course_art/piece_07_draw_polygons.inc"
};
static Sint32 piece_07_draw_vertices[3771] = {
#include "cart/course_art/piece_07_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_07_draw_model = {
#include "cart/course_art/piece_07_draw_model.inc"
};
static NJS_CNK_OBJECT piece_07_draw = {
#include "cart/course_art/piece_07_draw.inc"
};
static Sint16 piece_08_draw_polygons[4291] = {
#include "cart/course_art/piece_08_draw_polygons.inc"
};
static Sint32 piece_08_draw_vertices[3763] = {
#include "cart/course_art/piece_08_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_08_draw_model = {
#include "cart/course_art/piece_08_draw_model.inc"
};
static NJS_CNK_OBJECT piece_08_draw = {
#include "cart/course_art/piece_08_draw.inc"
};
static Sint16 piece_09_draw_polygons[10333] = {
#include "cart/course_art/piece_09_draw_polygons.inc"
};
static Sint32 piece_09_draw_vertices[10339] = {
#include "cart/course_art/piece_09_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_09_draw_model = {
#include "cart/course_art/piece_09_draw_model.inc"
};
static NJS_CNK_OBJECT piece_09_draw = {
#include "cart/course_art/piece_09_draw.inc"
};
static Sint16 piece_10_draw_polygons[175] = {
#include "cart/course_art/piece_10_draw_polygons.inc"
};
static Sint32 piece_10_draw_vertices[147] = {
#include "cart/course_art/piece_10_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_10_draw_model = {
#include "cart/course_art/piece_10_draw_model.inc"
};
static NJS_CNK_OBJECT piece_10_draw = {
#include "cart/course_art/piece_10_draw.inc"
};
static Sint16 piece_11_draw_polygons[5147] = {
#include "cart/course_art/piece_11_draw_polygons.inc"
};
static Sint32 piece_11_draw_vertices[4599] = {
#include "cart/course_art/piece_11_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_11_draw_model = {
#include "cart/course_art/piece_11_draw_model.inc"
};
static NJS_CNK_OBJECT piece_11_draw = {
#include "cart/course_art/piece_11_draw.inc"
};
static Sint16 piece_12_draw_polygons[1063] = {
#include "cart/course_art/piece_12_draw_polygons.inc"
};
static Sint32 piece_12_draw_vertices[931] = {
#include "cart/course_art/piece_12_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_12_draw_model = {
#include "cart/course_art/piece_12_draw_model.inc"
};
static NJS_CNK_OBJECT piece_12_draw = {
#include "cart/course_art/piece_12_draw.inc"
};
static Sint16 piece_13_draw_polygons[1065] = {
#include "cart/course_art/piece_13_draw_polygons.inc"
};
static Sint32 piece_13_draw_vertices[931] = {
#include "cart/course_art/piece_13_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_13_draw_model = {
#include "cart/course_art/piece_13_draw_model.inc"
};
static NJS_CNK_OBJECT piece_13_draw = {
#include "cart/course_art/piece_13_draw.inc"
};
static Sint16 piece_14_draw_polygons[1369] = {
#include "cart/course_art/piece_14_draw_polygons.inc"
};
static Sint32 piece_14_draw_vertices[1219] = {
#include "cart/course_art/piece_14_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_14_draw_model = {
#include "cart/course_art/piece_14_draw_model.inc"
};
static NJS_CNK_OBJECT piece_14_draw = {
#include "cart/course_art/piece_14_draw.inc"
};
static Sint16 piece_15_draw_polygons[1307] = {
#include "cart/course_art/piece_15_draw_polygons.inc"
};
static Sint32 piece_15_draw_vertices[1219] = {
#include "cart/course_art/piece_15_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_15_draw_model = {
#include "cart/course_art/piece_15_draw_model.inc"
};
static NJS_CNK_OBJECT piece_15_draw = {
#include "cart/course_art/piece_15_draw.inc"
};
static Sint16 piece_16_draw_polygons[1043] = {
#include "cart/course_art/piece_16_draw_polygons.inc"
};
static Sint32 piece_16_draw_vertices[931] = {
#include "cart/course_art/piece_16_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_16_draw_model = {
#include "cart/course_art/piece_16_draw_model.inc"
};
static NJS_CNK_OBJECT piece_16_draw = {
#include "cart/course_art/piece_16_draw.inc"
};
static Sint16 piece_17_draw_polygons[1995] = {
#include "cart/course_art/piece_17_draw_polygons.inc"
};
static Sint32 piece_17_draw_vertices[1735] = {
#include "cart/course_art/piece_17_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_17_draw_model = {
#include "cart/course_art/piece_17_draw_model.inc"
};
static NJS_CNK_OBJECT piece_17_draw = {
#include "cart/course_art/piece_17_draw.inc"
};
static Sint16 piece_18_draw_polygons[2011] = {
#include "cart/course_art/piece_18_draw_polygons.inc"
};
static Sint32 piece_18_draw_vertices[1735] = {
#include "cart/course_art/piece_18_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_18_draw_model = {
#include "cart/course_art/piece_18_draw_model.inc"
};
static NJS_CNK_OBJECT piece_18_draw = {
#include "cart/course_art/piece_18_draw.inc"
};
static Sint16 piece_19_draw_polygons[1175] = {
#include "cart/course_art/piece_19_draw_polygons.inc"
};
static Sint32 piece_19_draw_vertices[1075] = {
#include "cart/course_art/piece_19_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_19_draw_model = {
#include "cart/course_art/piece_19_draw_model.inc"
};
static NJS_CNK_OBJECT piece_19_draw = {
#include "cart/course_art/piece_19_draw.inc"
};
static Sint16 piece_20_draw_polygons[1201] = {
#include "cart/course_art/piece_20_draw_polygons.inc"
};
static Sint32 piece_20_draw_vertices[1075] = {
#include "cart/course_art/piece_20_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_20_draw_model = {
#include "cart/course_art/piece_20_draw_model.inc"
};
static NJS_CNK_OBJECT piece_20_draw = {
#include "cart/course_art/piece_20_draw.inc"
};
static Sint16 piece_21_draw_polygons[1183] = {
#include "cart/course_art/piece_21_draw_polygons.inc"
};
static Sint32 piece_21_draw_vertices[1075] = {
#include "cart/course_art/piece_21_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_21_draw_model = {
#include "cart/course_art/piece_21_draw_model.inc"
};
static NJS_CNK_OBJECT piece_21_draw = {
#include "cart/course_art/piece_21_draw.inc"
};
static Sint16 piece_22_draw_polygons[1183] = {
#include "cart/course_art/piece_22_draw_polygons.inc"
};
static Sint32 piece_22_draw_vertices[1075] = {
#include "cart/course_art/piece_22_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_22_draw_model = {
#include "cart/course_art/piece_22_draw_model.inc"
};
static NJS_CNK_OBJECT piece_22_draw = {
#include "cart/course_art/piece_22_draw.inc"
};
static Sint16 piece_23_draw_polygons[1215] = {
#include "cart/course_art/piece_23_draw_polygons.inc"
};
static Sint32 piece_23_draw_vertices[1075] = {
#include "cart/course_art/piece_23_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_23_draw_model = {
#include "cart/course_art/piece_23_draw_model.inc"
};
static NJS_CNK_OBJECT piece_23_draw = {
#include "cart/course_art/piece_23_draw.inc"
};
static Sint16 piece_24_draw_polygons[1207] = {
#include "cart/course_art/piece_24_draw_polygons.inc"
};
static Sint32 piece_24_draw_vertices[1075] = {
#include "cart/course_art/piece_24_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_24_draw_model = {
#include "cart/course_art/piece_24_draw_model.inc"
};
static NJS_CNK_OBJECT piece_24_draw = {
#include "cart/course_art/piece_24_draw.inc"
};
static Sint16 piece_28_draw_polygons[217] = {
#include "cart/course_art/piece_28_draw_polygons.inc"
};
static Sint32 piece_28_draw_vertices[191] = {
#include "cart/course_art/piece_28_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_28_draw_model = {
#include "cart/course_art/piece_28_draw_model.inc"
};
static NJS_CNK_OBJECT piece_28_draw = {
#include "cart/course_art/piece_28_draw.inc"
};
static Sint16 piece_29_draw_polygons[249] = {
#include "cart/course_art/piece_29_draw_polygons.inc"
};
static Sint32 piece_29_draw_vertices[219] = {
#include "cart/course_art/piece_29_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_29_draw_model = {
#include "cart/course_art/piece_29_draw_model.inc"
};
static NJS_CNK_OBJECT piece_29_draw = {
#include "cart/course_art/piece_29_draw.inc"
};
static Sint16 piece_30_draw_polygons[207] = {
#include "cart/course_art/piece_30_draw_polygons.inc"
};
static Sint32 piece_30_draw_vertices[179] = {
#include "cart/course_art/piece_30_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_30_draw_model = {
#include "cart/course_art/piece_30_draw_model.inc"
};
static NJS_CNK_OBJECT piece_30_draw = {
#include "cart/course_art/piece_30_draw.inc"
};
static Sint16 piece_31_draw_polygons[219] = {
#include "cart/course_art/piece_31_draw_polygons.inc"
};
static Sint32 piece_31_draw_vertices[219] = {
#include "cart/course_art/piece_31_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_31_draw_model = {
#include "cart/course_art/piece_31_draw_model.inc"
};
static NJS_CNK_OBJECT piece_31_draw = {
#include "cart/course_art/piece_31_draw.inc"
};
static Sint16 piece_32_draw_polygons[3257] = {
#include "cart/course_art/piece_32_draw_polygons.inc"
};
static Sint32 piece_32_draw_vertices[2971] = {
#include "cart/course_art/piece_32_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_32_draw_model = {
#include "cart/course_art/piece_32_draw_model.inc"
};
static NJS_CNK_OBJECT piece_32_draw = {
#include "cart/course_art/piece_32_draw.inc"
};
static Sint16 piece_33_draw_polygons[3255] = {
#include "cart/course_art/piece_33_draw_polygons.inc"
};
static Sint32 piece_33_draw_vertices[2967] = {
#include "cart/course_art/piece_33_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_33_draw_model = {
#include "cart/course_art/piece_33_draw_model.inc"
};
static NJS_CNK_OBJECT piece_33_draw = {
#include "cart/course_art/piece_33_draw.inc"
};
static Sint16 piece_34_draw_polygons[737] = {
#include "cart/course_art/piece_34_draw_polygons.inc"
};
static Sint32 piece_34_draw_vertices[631] = {
#include "cart/course_art/piece_34_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_34_draw_model = {
#include "cart/course_art/piece_34_draw_model.inc"
};
static NJS_CNK_OBJECT piece_34_draw = {
#include "cart/course_art/piece_34_draw.inc"
};
static Sint16 piece_35_draw_polygons[717] = {
#include "cart/course_art/piece_35_draw_polygons.inc"
};
static Sint32 piece_35_draw_vertices[635] = {
#include "cart/course_art/piece_35_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_35_draw_model = {
#include "cart/course_art/piece_35_draw_model.inc"
};
static NJS_CNK_OBJECT piece_35_draw = {
#include "cart/course_art/piece_35_draw.inc"
};
static Sint16 piece_36_draw_polygons[3173] = {
#include "cart/course_art/piece_36_draw_polygons.inc"
};
static Sint32 piece_36_draw_vertices[2799] = {
#include "cart/course_art/piece_36_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_36_draw_model = {
#include "cart/course_art/piece_36_draw_model.inc"
};
static NJS_CNK_OBJECT piece_36_draw = {
#include "cart/course_art/piece_36_draw.inc"
};
static Sint16 piece_37_draw_polygons[3189] = {
#include "cart/course_art/piece_37_draw_polygons.inc"
};
static Sint32 piece_37_draw_vertices[2799] = {
#include "cart/course_art/piece_37_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_37_draw_model = {
#include "cart/course_art/piece_37_draw_model.inc"
};
static NJS_CNK_OBJECT piece_37_draw = {
#include "cart/course_art/piece_37_draw.inc"
};
static Sint16 piece_38_draw_polygons[657] = {
#include "cart/course_art/piece_38_draw_polygons.inc"
};
static Sint32 piece_38_draw_vertices[551] = {
#include "cart/course_art/piece_38_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_38_draw_model = {
#include "cart/course_art/piece_38_draw_model.inc"
};
static NJS_CNK_OBJECT piece_38_draw = {
#include "cart/course_art/piece_38_draw.inc"
};
static Sint16 piece_39_draw_polygons[661] = {
#include "cart/course_art/piece_39_draw_polygons.inc"
};
static Sint32 piece_39_draw_vertices[551] = {
#include "cart/course_art/piece_39_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_39_draw_model = {
#include "cart/course_art/piece_39_draw_model.inc"
};
static NJS_CNK_OBJECT piece_39_draw = {
#include "cart/course_art/piece_39_draw.inc"
};
static Sint16 piece_40_draw_polygons[3167] = {
#include "cart/course_art/piece_40_draw_polygons.inc"
};
static Sint32 piece_40_draw_vertices[2799] = {
#include "cart/course_art/piece_40_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_40_draw_model = {
#include "cart/course_art/piece_40_draw_model.inc"
};
static NJS_CNK_OBJECT piece_40_draw = {
#include "cart/course_art/piece_40_draw.inc"
};
static Sint16 piece_41_draw_polygons[3203] = {
#include "cart/course_art/piece_41_draw_polygons.inc"
};
static Sint32 piece_41_draw_vertices[2799] = {
#include "cart/course_art/piece_41_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_41_draw_model = {
#include "cart/course_art/piece_41_draw_model.inc"
};
static NJS_CNK_OBJECT piece_41_draw = {
#include "cart/course_art/piece_41_draw.inc"
};
static Sint16 piece_42_draw_polygons[661] = {
#include "cart/course_art/piece_42_draw_polygons.inc"
};
static Sint32 piece_42_draw_vertices[551] = {
#include "cart/course_art/piece_42_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_42_draw_model = {
#include "cart/course_art/piece_42_draw_model.inc"
};
static NJS_CNK_OBJECT piece_42_draw = {
#include "cart/course_art/piece_42_draw.inc"
};
static Sint16 piece_43_draw_polygons[675] = {
#include "cart/course_art/piece_43_draw_polygons.inc"
};
static Sint32 piece_43_draw_vertices[551] = {
#include "cart/course_art/piece_43_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_43_draw_model = {
#include "cart/course_art/piece_43_draw_model.inc"
};
static NJS_CNK_OBJECT piece_43_draw = {
#include "cart/course_art/piece_43_draw.inc"
};
static Sint16 piece_44_draw_polygons[1439] = {
#include "cart/course_art/piece_44_draw_polygons.inc"
};
static Sint32 piece_44_draw_vertices[1199] = {
#include "cart/course_art/piece_44_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_44_draw_model = {
#include "cart/course_art/piece_44_draw_model.inc"
};
static NJS_CNK_OBJECT piece_44_draw = {
#include "cart/course_art/piece_44_draw.inc"
};
static Sint16 piece_45_draw_polygons[1439] = {
#include "cart/course_art/piece_45_draw_polygons.inc"
};
static Sint32 piece_45_draw_vertices[1207] = {
#include "cart/course_art/piece_45_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_45_draw_model = {
#include "cart/course_art/piece_45_draw_model.inc"
};
static NJS_CNK_OBJECT piece_45_draw = {
#include "cart/course_art/piece_45_draw.inc"
};
static Sint16 piece_46_draw_polygons[1909] = {
#include "cart/course_art/piece_46_draw_polygons.inc"
};
static Sint32 piece_46_draw_vertices[1595] = {
#include "cart/course_art/piece_46_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_46_draw_model = {
#include "cart/course_art/piece_46_draw_model.inc"
};
static NJS_CNK_OBJECT piece_46_draw = {
#include "cart/course_art/piece_46_draw.inc"
};
static Sint16 piece_47_draw_polygons[1901] = {
#include "cart/course_art/piece_47_draw_polygons.inc"
};
static Sint32 piece_47_draw_vertices[1595] = {
#include "cart/course_art/piece_47_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_47_draw_model = {
#include "cart/course_art/piece_47_draw_model.inc"
};
static NJS_CNK_OBJECT piece_47_draw = {
#include "cart/course_art/piece_47_draw.inc"
};
static Sint16 piece_48_draw_polygons[1937] = {
#include "cart/course_art/piece_48_draw_polygons.inc"
};
static Sint32 piece_48_draw_vertices[1587] = {
#include "cart/course_art/piece_48_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_48_draw_model = {
#include "cart/course_art/piece_48_draw_model.inc"
};
static NJS_CNK_OBJECT piece_48_draw = {
#include "cart/course_art/piece_48_draw.inc"
};
static Sint16 piece_49_draw_polygons[1951] = {
#include "cart/course_art/piece_49_draw_polygons.inc"
};
static Sint32 piece_49_draw_vertices[1607] = {
#include "cart/course_art/piece_49_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_49_draw_model = {
#include "cart/course_art/piece_49_draw_model.inc"
};
static NJS_CNK_OBJECT piece_49_draw = {
#include "cart/course_art/piece_49_draw.inc"
};
static Sint16 piece_50_draw_polygons[741] = {
#include "cart/course_art/piece_50_draw_polygons.inc"
};
static Sint32 piece_50_draw_vertices[551] = {
#include "cart/course_art/piece_50_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_50_draw_model = {
#include "cart/course_art/piece_50_draw_model.inc"
};
static NJS_CNK_OBJECT piece_50_draw = {
#include "cart/course_art/piece_50_draw.inc"
};
static Sint16 piece_51_draw_polygons[691] = {
#include "cart/course_art/piece_51_draw_polygons.inc"
};
static Sint32 piece_51_draw_vertices[515] = {
#include "cart/course_art/piece_51_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_51_draw_model = {
#include "cart/course_art/piece_51_draw_model.inc"
};
static NJS_CNK_OBJECT piece_51_draw = {
#include "cart/course_art/piece_51_draw.inc"
};
static Sint16 piece_52_draw_polygons[493] = {
#include "cart/course_art/piece_52_draw_polygons.inc"
};
static Sint32 piece_52_draw_vertices[383] = {
#include "cart/course_art/piece_52_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_52_draw_model = {
#include "cart/course_art/piece_52_draw_model.inc"
};
static NJS_CNK_OBJECT piece_52_draw = {
#include "cart/course_art/piece_52_draw.inc"
};
static Sint16 piece_53_draw_polygons[715] = {
#include "cart/course_art/piece_53_draw_polygons.inc"
};
static Sint32 piece_53_draw_vertices[543] = {
#include "cart/course_art/piece_53_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_53_draw_model = {
#include "cart/course_art/piece_53_draw_model.inc"
};
static NJS_CNK_OBJECT piece_53_draw = {
#include "cart/course_art/piece_53_draw.inc"
};
static Sint16 piece_54_draw_polygons[715] = {
#include "cart/course_art/piece_54_draw_polygons.inc"
};
static Sint32 piece_54_draw_vertices[543] = {
#include "cart/course_art/piece_54_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_54_draw_model = {
#include "cart/course_art/piece_54_draw_model.inc"
};
static NJS_CNK_OBJECT piece_54_draw = {
#include "cart/course_art/piece_54_draw.inc"
};
static Sint16 piece_55_draw_polygons[473] = {
#include "cart/course_art/piece_55_draw_polygons.inc"
};
static Sint32 piece_55_draw_vertices[339] = {
#include "cart/course_art/piece_55_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_55_draw_model = {
#include "cart/course_art/piece_55_draw_model.inc"
};
static NJS_CNK_OBJECT piece_55_draw = {
#include "cart/course_art/piece_55_draw.inc"
};
static Sint16 piece_56_draw_polygons[475] = {
#include "cart/course_art/piece_56_draw_polygons.inc"
};
static Sint32 piece_56_draw_vertices[339] = {
#include "cart/course_art/piece_56_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_56_draw_model = {
#include "cart/course_art/piece_56_draw_model.inc"
};
static NJS_CNK_OBJECT piece_56_draw = {
#include "cart/course_art/piece_56_draw.inc"
};
static Sint16 piece_57_draw_polygons[405] = {
#include "cart/course_art/piece_57_draw_polygons.inc"
};
static Sint32 piece_57_draw_vertices[323] = {
#include "cart/course_art/piece_57_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_57_draw_model = {
#include "cart/course_art/piece_57_draw_model.inc"
};
static NJS_CNK_OBJECT piece_57_draw = {
#include "cart/course_art/piece_57_draw.inc"
};
static Sint16 piece_58_draw_polygons[643] = {
#include "cart/course_art/piece_58_draw_polygons.inc"
};
static Sint32 piece_58_draw_vertices[503] = {
#include "cart/course_art/piece_58_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_58_draw_model = {
#include "cart/course_art/piece_58_draw_model.inc"
};
static NJS_CNK_OBJECT piece_58_draw = {
#include "cart/course_art/piece_58_draw.inc"
};
static Sint16 piece_59_draw_polygons[639] = {
#include "cart/course_art/piece_59_draw_polygons.inc"
};
static Sint32 piece_59_draw_vertices[503] = {
#include "cart/course_art/piece_59_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_59_draw_model = {
#include "cart/course_art/piece_59_draw_model.inc"
};
static NJS_CNK_OBJECT piece_59_draw = {
#include "cart/course_art/piece_59_draw.inc"
};
static Sint16 piece_60_draw_polygons[709] = {
#include "cart/course_art/piece_60_draw_polygons.inc"
};
static Sint32 piece_60_draw_vertices[543] = {
#include "cart/course_art/piece_60_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_60_draw_model = {
#include "cart/course_art/piece_60_draw_model.inc"
};
static NJS_CNK_OBJECT piece_60_draw = {
#include "cart/course_art/piece_60_draw.inc"
};
static Sint16 piece_61_draw_polygons[395] = {
#include "cart/course_art/piece_61_draw_polygons.inc"
};
static Sint32 piece_61_draw_vertices[287] = {
#include "cart/course_art/piece_61_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_61_draw_model = {
#include "cart/course_art/piece_61_draw_model.inc"
};
static NJS_CNK_OBJECT piece_61_draw = {
#include "cart/course_art/piece_61_draw.inc"
};
static Sint16 piece_62_draw_polygons[707] = {
#include "cart/course_art/piece_62_draw_polygons.inc"
};
static Sint32 piece_62_draw_vertices[543] = {
#include "cart/course_art/piece_62_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_62_draw_model = {
#include "cart/course_art/piece_62_draw_model.inc"
};
static NJS_CNK_OBJECT piece_62_draw = {
#include "cart/course_art/piece_62_draw.inc"
};
static Sint16 piece_63_draw_polygons[689] = {
#include "cart/course_art/piece_63_draw_polygons.inc"
};
static Sint32 piece_63_draw_vertices[543] = {
#include "cart/course_art/piece_63_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_63_draw_model = {
#include "cart/course_art/piece_63_draw_model.inc"
};
static NJS_CNK_OBJECT piece_63_draw = {
#include "cart/course_art/piece_63_draw.inc"
};
static Sint16 piece_64_draw_polygons[395] = {
#include "cart/course_art/piece_64_draw_polygons.inc"
};
static Sint32 piece_64_draw_vertices[287] = {
#include "cart/course_art/piece_64_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_64_draw_model = {
#include "cart/course_art/piece_64_draw_model.inc"
};
static NJS_CNK_OBJECT piece_64_draw = {
#include "cart/course_art/piece_64_draw.inc"
};
static Sint16 piece_65_draw_polygons[689] = {
#include "cart/course_art/piece_65_draw_polygons.inc"
};
static Sint32 piece_65_draw_vertices[543] = {
#include "cart/course_art/piece_65_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_65_draw_model = {
#include "cart/course_art/piece_65_draw_model.inc"
};
static NJS_CNK_OBJECT piece_65_draw = {
#include "cart/course_art/piece_65_draw.inc"
};
static Sint16 piece_66_draw_polygons[641] = {
#include "cart/course_art/piece_66_draw_polygons.inc"
};
static Sint32 piece_66_draw_vertices[503] = {
#include "cart/course_art/piece_66_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_66_draw_model = {
#include "cart/course_art/piece_66_draw_model.inc"
};
static NJS_CNK_OBJECT piece_66_draw = {
#include "cart/course_art/piece_66_draw.inc"
};
static Sint16 piece_67_draw_polygons[351] = {
#include "cart/course_art/piece_67_draw_polygons.inc"
};
static Sint32 piece_67_draw_vertices[263] = {
#include "cart/course_art/piece_67_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_67_draw_model = {
#include "cart/course_art/piece_67_draw_model.inc"
};
static NJS_CNK_OBJECT piece_67_draw = {
#include "cart/course_art/piece_67_draw.inc"
};
static Sint16 piece_68_draw_polygons[641] = {
#include "cart/course_art/piece_68_draw_polygons.inc"
};
static Sint32 piece_68_draw_vertices[503] = {
#include "cart/course_art/piece_68_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_68_draw_model = {
#include "cart/course_art/piece_68_draw_model.inc"
};
static NJS_CNK_OBJECT piece_68_draw = {
#include "cart/course_art/piece_68_draw.inc"
};
static Sint16 piece_69_draw_polygons[641] = {
#include "cart/course_art/piece_69_draw_polygons.inc"
};
static Sint32 piece_69_draw_vertices[503] = {
#include "cart/course_art/piece_69_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_69_draw_model = {
#include "cart/course_art/piece_69_draw_model.inc"
};
static NJS_CNK_OBJECT piece_69_draw = {
#include "cart/course_art/piece_69_draw.inc"
};
static Sint16 piece_70_draw_polygons[351] = {
#include "cart/course_art/piece_70_draw_polygons.inc"
};
static Sint32 piece_70_draw_vertices[263] = {
#include "cart/course_art/piece_70_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_70_draw_model = {
#include "cart/course_art/piece_70_draw_model.inc"
};
static NJS_CNK_OBJECT piece_70_draw = {
#include "cart/course_art/piece_70_draw.inc"
};
static Sint16 piece_71_draw_polygons[623] = {
#include "cart/course_art/piece_71_draw_polygons.inc"
};
static Sint32 piece_71_draw_vertices[503] = {
#include "cart/course_art/piece_71_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_71_draw_model = {
#include "cart/course_art/piece_71_draw_model.inc"
};
static NJS_CNK_OBJECT piece_71_draw = {
#include "cart/course_art/piece_71_draw.inc"
};
static Sint16 piece_72_draw_polygons[1479] = {
#include "cart/course_art/piece_72_draw_polygons.inc"
};
static Sint32 piece_72_draw_vertices[1143] = {
#include "cart/course_art/piece_72_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_72_draw_model = {
#include "cart/course_art/piece_72_draw_model.inc"
};
static NJS_CNK_OBJECT piece_72_draw = {
#include "cart/course_art/piece_72_draw.inc"
};
static Sint16 piece_73_draw_polygons[1199] = {
#include "cart/course_art/piece_73_draw_polygons.inc"
};
static Sint32 piece_73_draw_vertices[915] = {
#include "cart/course_art/piece_73_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_73_draw_model = {
#include "cart/course_art/piece_73_draw_model.inc"
};
static NJS_CNK_OBJECT piece_73_draw = {
#include "cart/course_art/piece_73_draw.inc"
};
static Sint16 piece_74_draw_polygons[1251] = {
#include "cart/course_art/piece_74_draw_polygons.inc"
};
static Sint32 piece_74_draw_vertices[899] = {
#include "cart/course_art/piece_74_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_74_draw_model = {
#include "cart/course_art/piece_74_draw_model.inc"
};
static NJS_CNK_OBJECT piece_74_draw = {
#include "cart/course_art/piece_74_draw.inc"
};
static Sint16 piece_75_draw_polygons[1475] = {
#include "cart/course_art/piece_75_draw_polygons.inc"
};
static Sint32 piece_75_draw_vertices[1147] = {
#include "cart/course_art/piece_75_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_75_draw_model = {
#include "cart/course_art/piece_75_draw_model.inc"
};
static NJS_CNK_OBJECT piece_75_draw = {
#include "cart/course_art/piece_75_draw.inc"
};
static Sint16 piece_76_draw_polygons[1135] = {
#include "cart/course_art/piece_76_draw_polygons.inc"
};
static Sint32 piece_76_draw_vertices[911] = {
#include "cart/course_art/piece_76_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_76_draw_model = {
#include "cart/course_art/piece_76_draw_model.inc"
};
static NJS_CNK_OBJECT piece_76_draw = {
#include "cart/course_art/piece_76_draw.inc"
};
static Sint16 piece_77_draw_polygons[1119] = {
#include "cart/course_art/piece_77_draw_polygons.inc"
};
static Sint32 piece_77_draw_vertices[899] = {
#include "cart/course_art/piece_77_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_77_draw_model = {
#include "cart/course_art/piece_77_draw_model.inc"
};
static NJS_CNK_OBJECT piece_77_draw = {
#include "cart/course_art/piece_77_draw.inc"
};
static Sint16 piece_78_draw_polygons[293] = {
#include "cart/course_art/piece_78_draw_polygons.inc"
};
static Sint32 piece_78_draw_vertices[243] = {
#include "cart/course_art/piece_78_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_78_draw_model = {
#include "cart/course_art/piece_78_draw_model.inc"
};
static NJS_CNK_OBJECT piece_78_draw = {
#include "cart/course_art/piece_78_draw.inc"
};
static Sint16 piece_79_draw_polygons[293] = {
#include "cart/course_art/piece_79_draw_polygons.inc"
};
static Sint32 piece_79_draw_vertices[243] = {
#include "cart/course_art/piece_79_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_79_draw_model = {
#include "cart/course_art/piece_79_draw_model.inc"
};
static NJS_CNK_OBJECT piece_79_draw = {
#include "cart/course_art/piece_79_draw.inc"
};
static Sint16 piece_80_draw_polygons[10333] = {
#include "cart/course_art/piece_80_draw_polygons.inc"
};
static Sint32 piece_80_draw_vertices[10327] = {
#include "cart/course_art/piece_80_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_80_draw_model = {
#include "cart/course_art/piece_80_draw_model.inc"
};
static NJS_CNK_OBJECT piece_80_draw = {
#include "cart/course_art/piece_80_draw.inc"
};
static Sint16 piece_81_draw_polygons[175] = {
#include "cart/course_art/piece_81_draw_polygons.inc"
};
static Sint32 piece_81_draw_vertices[147] = {
#include "cart/course_art/piece_81_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_81_draw_model = {
#include "cart/course_art/piece_81_draw_model.inc"
};
static NJS_CNK_OBJECT piece_81_draw = {
#include "cart/course_art/piece_81_draw.inc"
};
static Sint16 piece_82_draw_polygons[175] = {
#include "cart/course_art/piece_82_draw_polygons.inc"
};
static Sint32 piece_82_draw_vertices[147] = {
#include "cart/course_art/piece_82_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_82_draw_model = {
#include "cart/course_art/piece_82_draw_model.inc"
};
static NJS_CNK_OBJECT piece_82_draw = {
#include "cart/course_art/piece_82_draw.inc"
};
static Sint16 piece_83_draw_polygons[175] = {
#include "cart/course_art/piece_83_draw_polygons.inc"
};
static Sint32 piece_83_draw_vertices[147] = {
#include "cart/course_art/piece_83_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_83_draw_model = {
#include "cart/course_art/piece_83_draw_model.inc"
};
static NJS_CNK_OBJECT piece_83_draw = {
#include "cart/course_art/piece_83_draw.inc"
};
static Sint16 piece_84_draw_polygons[175] = {
#include "cart/course_art/piece_84_draw_polygons.inc"
};
static Sint32 piece_84_draw_vertices[147] = {
#include "cart/course_art/piece_84_draw_vertices.inc"
};
static NJS_CNK_MODEL piece_84_draw_model = {
#include "cart/course_art/piece_84_draw_model.inc"
};
static NJS_CNK_OBJECT piece_84_draw = {
#include "cart/course_art/piece_84_draw.inc"
};
static NJS_MATERIAL piece_01_solid_materials[1] = {
#include "cart/course_art/piece_01_solid_materials.inc"
};
static Sint16 piece_01_solid_indices[44] = {
#include "cart/course_art/piece_01_solid_indices.inc"
};
static NJS_MESHSET piece_01_solid_meshsets[1] = {
#include "cart/course_art/piece_01_solid_meshsets.inc"
};
static NJS_POINT3 piece_01_solid_points[25] = {
#include "cart/course_art/piece_01_solid_points.inc"
};
static NJS_POINT3 piece_01_solid_normals[25] = {
#include "cart/course_art/piece_01_solid_normals.inc"
};
static NJS_MODEL piece_01_solid_model = {
#include "cart/course_art/piece_01_solid_model.inc"
};
static NJS_OBJECT piece_01_solid = {
#include "cart/course_art/piece_01_solid.inc"
};
static NJS_MATERIAL piece_02_solid_materials[1] = {
#include "cart/course_art/piece_02_solid_materials.inc"
};
static Sint16 piece_02_solid_indices[44] = {
#include "cart/course_art/piece_02_solid_indices.inc"
};
static NJS_MESHSET piece_02_solid_meshsets[1] = {
#include "cart/course_art/piece_02_solid_meshsets.inc"
};
static NJS_POINT3 piece_02_solid_points[25] = {
#include "cart/course_art/piece_02_solid_points.inc"
};
static NJS_POINT3 piece_02_solid_normals[25] = {
#include "cart/course_art/piece_02_solid_normals.inc"
};
static NJS_MODEL piece_02_solid_model = {
#include "cart/course_art/piece_02_solid_model.inc"
};
static NJS_OBJECT piece_02_solid = {
#include "cart/course_art/piece_02_solid.inc"
};
static NJS_MATERIAL piece_03_solid_materials[1] = {
#include "cart/course_art/piece_03_solid_materials.inc"
};
static Sint16 piece_03_solid_indices[44] = {
#include "cart/course_art/piece_03_solid_indices.inc"
};
static NJS_MESHSET piece_03_solid_meshsets[1] = {
#include "cart/course_art/piece_03_solid_meshsets.inc"
};
static NJS_POINT3 piece_03_solid_points[25] = {
#include "cart/course_art/piece_03_solid_points.inc"
};
static NJS_POINT3 piece_03_solid_normals[25] = {
#include "cart/course_art/piece_03_solid_normals.inc"
};
static NJS_MODEL piece_03_solid_model = {
#include "cart/course_art/piece_03_solid_model.inc"
};
static NJS_OBJECT piece_03_solid = {
#include "cart/course_art/piece_03_solid.inc"
};
static NJS_MATERIAL piece_04_solid_materials[1] = {
#include "cart/course_art/piece_04_solid_materials.inc"
};
static Sint16 piece_04_solid_indices[33] = {
#include "cart/course_art/piece_04_solid_indices.inc"
};
static NJS_MESHSET piece_04_solid_meshsets[1] = {
#include "cart/course_art/piece_04_solid_meshsets.inc"
};
static NJS_POINT3 piece_04_solid_points[20] = {
#include "cart/course_art/piece_04_solid_points.inc"
};
static NJS_POINT3 piece_04_solid_normals[20] = {
#include "cart/course_art/piece_04_solid_normals.inc"
};
static NJS_MODEL piece_04_solid_model = {
#include "cart/course_art/piece_04_solid_model.inc"
};
static NJS_OBJECT piece_04_solid = {
#include "cart/course_art/piece_04_solid.inc"
};
static NJS_MATERIAL piece_05_solid_materials[1] = {
#include "cart/course_art/piece_05_solid_materials.inc"
};
static Sint16 piece_05_solid_indices[63] = {
#include "cart/course_art/piece_05_solid_indices.inc"
};
static NJS_MESHSET piece_05_solid_meshsets[1] = {
#include "cart/course_art/piece_05_solid_meshsets.inc"
};
static NJS_POINT3 piece_05_solid_points[31] = {
#include "cart/course_art/piece_05_solid_points.inc"
};
static NJS_POINT3 piece_05_solid_normals[31] = {
#include "cart/course_art/piece_05_solid_normals.inc"
};
static NJS_MODEL piece_05_solid_model = {
#include "cart/course_art/piece_05_solid_model.inc"
};
static NJS_OBJECT piece_05_solid = {
#include "cart/course_art/piece_05_solid.inc"
};
static NJS_MATERIAL piece_06_solid_materials[1] = {
#include "cart/course_art/piece_06_solid_materials.inc"
};
static Sint16 piece_06_solid_indices[63] = {
#include "cart/course_art/piece_06_solid_indices.inc"
};
static NJS_MESHSET piece_06_solid_meshsets[1] = {
#include "cart/course_art/piece_06_solid_meshsets.inc"
};
static NJS_POINT3 piece_06_solid_points[31] = {
#include "cart/course_art/piece_06_solid_points.inc"
};
static NJS_POINT3 piece_06_solid_normals[31] = {
#include "cart/course_art/piece_06_solid_normals.inc"
};
static NJS_MODEL piece_06_solid_model = {
#include "cart/course_art/piece_06_solid_model.inc"
};
static NJS_OBJECT piece_06_solid = {
#include "cart/course_art/piece_06_solid.inc"
};
static NJS_MATERIAL piece_07_solid_materials[1] = {
#include "cart/course_art/piece_07_solid_materials.inc"
};
static Sint16 piece_07_solid_indices[212] = {
#include "cart/course_art/piece_07_solid_indices.inc"
};
static NJS_MESHSET piece_07_solid_meshsets[1] = {
#include "cart/course_art/piece_07_solid_meshsets.inc"
};
static NJS_POINT3 piece_07_solid_points[112] = {
#include "cart/course_art/piece_07_solid_points.inc"
};
static NJS_POINT3 piece_07_solid_normals[112] = {
#include "cart/course_art/piece_07_solid_normals.inc"
};
static NJS_MODEL piece_07_solid_model = {
#include "cart/course_art/piece_07_solid_model.inc"
};
static NJS_OBJECT piece_07_solid = {
#include "cart/course_art/piece_07_solid.inc"
};
static NJS_MATERIAL piece_08_solid_materials[1] = {
#include "cart/course_art/piece_08_solid_materials.inc"
};
static Sint16 piece_08_solid_indices[212] = {
#include "cart/course_art/piece_08_solid_indices.inc"
};
static NJS_MESHSET piece_08_solid_meshsets[1] = {
#include "cart/course_art/piece_08_solid_meshsets.inc"
};
static NJS_POINT3 piece_08_solid_points[112] = {
#include "cart/course_art/piece_08_solid_points.inc"
};
static NJS_POINT3 piece_08_solid_normals[112] = {
#include "cart/course_art/piece_08_solid_normals.inc"
};
static NJS_MODEL piece_08_solid_model = {
#include "cart/course_art/piece_08_solid_model.inc"
};
static NJS_OBJECT piece_08_solid = {
#include "cart/course_art/piece_08_solid.inc"
};
static NJS_MATERIAL piece_09_solid_materials[1] = {
#include "cart/course_art/piece_09_solid_materials.inc"
};
static Sint16 piece_09_solid_indices[92] = {
#include "cart/course_art/piece_09_solid_indices.inc"
};
static NJS_MESHSET piece_09_solid_meshsets[1] = {
#include "cart/course_art/piece_09_solid_meshsets.inc"
};
static NJS_POINT3 piece_09_solid_points[55] = {
#include "cart/course_art/piece_09_solid_points.inc"
};
static NJS_POINT3 piece_09_solid_normals[55] = {
#include "cart/course_art/piece_09_solid_normals.inc"
};
static NJS_MODEL piece_09_solid_model = {
#include "cart/course_art/piece_09_solid_model.inc"
};
static NJS_OBJECT piece_09_solid = {
#include "cart/course_art/piece_09_solid.inc"
};
static NJS_MATERIAL piece_10_solid_materials[1] = {
#include "cart/course_art/piece_10_solid_materials.inc"
};
static Sint16 piece_10_solid_indices[14] = {
#include "cart/course_art/piece_10_solid_indices.inc"
};
static NJS_MESHSET piece_10_solid_meshsets[1] = {
#include "cart/course_art/piece_10_solid_meshsets.inc"
};
static NJS_POINT3 piece_10_solid_points[10] = {
#include "cart/course_art/piece_10_solid_points.inc"
};
static NJS_POINT3 piece_10_solid_normals[10] = {
#include "cart/course_art/piece_10_solid_normals.inc"
};
static NJS_MODEL piece_10_solid_model = {
#include "cart/course_art/piece_10_solid_model.inc"
};
static NJS_OBJECT piece_10_solid = {
#include "cart/course_art/piece_10_solid.inc"
};
static NJS_MATERIAL piece_11_solid_materials[1] = {
#include "cart/course_art/piece_11_solid_materials.inc"
};
static Sint16 piece_11_solid_indices[389] = {
#include "cart/course_art/piece_11_solid_indices.inc"
};
static NJS_MESHSET piece_11_solid_meshsets[1] = {
#include "cart/course_art/piece_11_solid_meshsets.inc"
};
static NJS_POINT3 piece_11_solid_points[195] = {
#include "cart/course_art/piece_11_solid_points.inc"
};
static NJS_POINT3 piece_11_solid_normals[195] = {
#include "cart/course_art/piece_11_solid_normals.inc"
};
static NJS_MODEL piece_11_solid_model = {
#include "cart/course_art/piece_11_solid_model.inc"
};
static NJS_OBJECT piece_11_solid = {
#include "cart/course_art/piece_11_solid.inc"
};
static NJS_MATERIAL piece_12_solid_materials[1] = {
#include "cart/course_art/piece_12_solid_materials.inc"
};
static Sint16 piece_12_solid_indices[22] = {
#include "cart/course_art/piece_12_solid_indices.inc"
};
static NJS_MESHSET piece_12_solid_meshsets[1] = {
#include "cart/course_art/piece_12_solid_meshsets.inc"
};
static NJS_POINT3 piece_12_solid_points[15] = {
#include "cart/course_art/piece_12_solid_points.inc"
};
static NJS_POINT3 piece_12_solid_normals[15] = {
#include "cart/course_art/piece_12_solid_normals.inc"
};
static NJS_MODEL piece_12_solid_model = {
#include "cart/course_art/piece_12_solid_model.inc"
};
static NJS_OBJECT piece_12_solid = {
#include "cart/course_art/piece_12_solid.inc"
};
static NJS_MATERIAL piece_13_solid_materials[1] = {
#include "cart/course_art/piece_13_solid_materials.inc"
};
static Sint16 piece_13_solid_indices[22] = {
#include "cart/course_art/piece_13_solid_indices.inc"
};
static NJS_MESHSET piece_13_solid_meshsets[1] = {
#include "cart/course_art/piece_13_solid_meshsets.inc"
};
static NJS_POINT3 piece_13_solid_points[15] = {
#include "cart/course_art/piece_13_solid_points.inc"
};
static NJS_POINT3 piece_13_solid_normals[15] = {
#include "cart/course_art/piece_13_solid_normals.inc"
};
static NJS_MODEL piece_13_solid_model = {
#include "cart/course_art/piece_13_solid_model.inc"
};
static NJS_OBJECT piece_13_solid = {
#include "cart/course_art/piece_13_solid.inc"
};
static NJS_MATERIAL piece_14_solid_materials[1] = {
#include "cart/course_art/piece_14_solid_materials.inc"
};
static Sint16 piece_14_solid_indices[60] = {
#include "cart/course_art/piece_14_solid_indices.inc"
};
static NJS_MESHSET piece_14_solid_meshsets[1] = {
#include "cart/course_art/piece_14_solid_meshsets.inc"
};
static NJS_POINT3 piece_14_solid_points[35] = {
#include "cart/course_art/piece_14_solid_points.inc"
};
static NJS_POINT3 piece_14_solid_normals[35] = {
#include "cart/course_art/piece_14_solid_normals.inc"
};
static NJS_MODEL piece_14_solid_model = {
#include "cart/course_art/piece_14_solid_model.inc"
};
static NJS_OBJECT piece_14_solid = {
#include "cart/course_art/piece_14_solid.inc"
};
static NJS_MATERIAL piece_15_solid_materials[1] = {
#include "cart/course_art/piece_15_solid_materials.inc"
};
static Sint16 piece_15_solid_indices[60] = {
#include "cart/course_art/piece_15_solid_indices.inc"
};
static NJS_MESHSET piece_15_solid_meshsets[1] = {
#include "cart/course_art/piece_15_solid_meshsets.inc"
};
static NJS_POINT3 piece_15_solid_points[35] = {
#include "cart/course_art/piece_15_solid_points.inc"
};
static NJS_POINT3 piece_15_solid_normals[35] = {
#include "cart/course_art/piece_15_solid_normals.inc"
};
static NJS_MODEL piece_15_solid_model = {
#include "cart/course_art/piece_15_solid_model.inc"
};
static NJS_OBJECT piece_15_solid = {
#include "cart/course_art/piece_15_solid.inc"
};
static NJS_MATERIAL piece_16_solid_materials[1] = {
#include "cart/course_art/piece_16_solid_materials.inc"
};
static Sint16 piece_16_solid_indices[22] = {
#include "cart/course_art/piece_16_solid_indices.inc"
};
static NJS_MESHSET piece_16_solid_meshsets[1] = {
#include "cart/course_art/piece_16_solid_meshsets.inc"
};
static NJS_POINT3 piece_16_solid_points[15] = {
#include "cart/course_art/piece_16_solid_points.inc"
};
static NJS_POINT3 piece_16_solid_normals[15] = {
#include "cart/course_art/piece_16_solid_normals.inc"
};
static NJS_MODEL piece_16_solid_model = {
#include "cart/course_art/piece_16_solid_model.inc"
};
static NJS_OBJECT piece_16_solid = {
#include "cart/course_art/piece_16_solid.inc"
};
static NJS_MATERIAL piece_17_solid_materials[1] = {
#include "cart/course_art/piece_17_solid_materials.inc"
};
static Sint16 piece_17_solid_indices[60] = {
#include "cart/course_art/piece_17_solid_indices.inc"
};
static NJS_MESHSET piece_17_solid_meshsets[1] = {
#include "cart/course_art/piece_17_solid_meshsets.inc"
};
static NJS_POINT3 piece_17_solid_points[35] = {
#include "cart/course_art/piece_17_solid_points.inc"
};
static NJS_POINT3 piece_17_solid_normals[35] = {
#include "cart/course_art/piece_17_solid_normals.inc"
};
static NJS_MODEL piece_17_solid_model = {
#include "cart/course_art/piece_17_solid_model.inc"
};
static NJS_OBJECT piece_17_solid = {
#include "cart/course_art/piece_17_solid.inc"
};
static NJS_MATERIAL piece_18_solid_materials[1] = {
#include "cart/course_art/piece_18_solid_materials.inc"
};
static Sint16 piece_18_solid_indices[60] = {
#include "cart/course_art/piece_18_solid_indices.inc"
};
static NJS_MESHSET piece_18_solid_meshsets[1] = {
#include "cart/course_art/piece_18_solid_meshsets.inc"
};
static NJS_POINT3 piece_18_solid_points[35] = {
#include "cart/course_art/piece_18_solid_points.inc"
};
static NJS_POINT3 piece_18_solid_normals[35] = {
#include "cart/course_art/piece_18_solid_normals.inc"
};
static NJS_MODEL piece_18_solid_model = {
#include "cart/course_art/piece_18_solid_model.inc"
};
static NJS_OBJECT piece_18_solid = {
#include "cart/course_art/piece_18_solid.inc"
};
static NJS_MATERIAL piece_19_solid_materials[1] = {
#include "cart/course_art/piece_19_solid_materials.inc"
};
static Sint16 piece_19_solid_indices[44] = {
#include "cart/course_art/piece_19_solid_indices.inc"
};
static NJS_MESHSET piece_19_solid_meshsets[1] = {
#include "cart/course_art/piece_19_solid_meshsets.inc"
};
static NJS_POINT3 piece_19_solid_points[25] = {
#include "cart/course_art/piece_19_solid_points.inc"
};
static NJS_POINT3 piece_19_solid_normals[25] = {
#include "cart/course_art/piece_19_solid_normals.inc"
};
static NJS_MODEL piece_19_solid_model = {
#include "cart/course_art/piece_19_solid_model.inc"
};
static NJS_OBJECT piece_19_solid = {
#include "cart/course_art/piece_19_solid.inc"
};
static NJS_MATERIAL piece_20_solid_materials[1] = {
#include "cart/course_art/piece_20_solid_materials.inc"
};
static Sint16 piece_20_solid_indices[60] = {
#include "cart/course_art/piece_20_solid_indices.inc"
};
static NJS_MESHSET piece_20_solid_meshsets[1] = {
#include "cart/course_art/piece_20_solid_meshsets.inc"
};
static NJS_POINT3 piece_20_solid_points[35] = {
#include "cart/course_art/piece_20_solid_points.inc"
};
static NJS_POINT3 piece_20_solid_normals[35] = {
#include "cart/course_art/piece_20_solid_normals.inc"
};
static NJS_MODEL piece_20_solid_model = {
#include "cart/course_art/piece_20_solid_model.inc"
};
static NJS_OBJECT piece_20_solid = {
#include "cart/course_art/piece_20_solid.inc"
};
static NJS_MATERIAL piece_21_solid_materials[1] = {
#include "cart/course_art/piece_21_solid_materials.inc"
};
static Sint16 piece_21_solid_indices[44] = {
#include "cart/course_art/piece_21_solid_indices.inc"
};
static NJS_MESHSET piece_21_solid_meshsets[1] = {
#include "cart/course_art/piece_21_solid_meshsets.inc"
};
static NJS_POINT3 piece_21_solid_points[25] = {
#include "cart/course_art/piece_21_solid_points.inc"
};
static NJS_POINT3 piece_21_solid_normals[25] = {
#include "cart/course_art/piece_21_solid_normals.inc"
};
static NJS_MODEL piece_21_solid_model = {
#include "cart/course_art/piece_21_solid_model.inc"
};
static NJS_OBJECT piece_21_solid = {
#include "cart/course_art/piece_21_solid.inc"
};
static NJS_MATERIAL piece_22_solid_materials[1] = {
#include "cart/course_art/piece_22_solid_materials.inc"
};
static Sint16 piece_22_solid_indices[44] = {
#include "cart/course_art/piece_22_solid_indices.inc"
};
static NJS_MESHSET piece_22_solid_meshsets[1] = {
#include "cart/course_art/piece_22_solid_meshsets.inc"
};
static NJS_POINT3 piece_22_solid_points[25] = {
#include "cart/course_art/piece_22_solid_points.inc"
};
static NJS_POINT3 piece_22_solid_normals[25] = {
#include "cart/course_art/piece_22_solid_normals.inc"
};
static NJS_MODEL piece_22_solid_model = {
#include "cart/course_art/piece_22_solid_model.inc"
};
static NJS_OBJECT piece_22_solid = {
#include "cart/course_art/piece_22_solid.inc"
};
static NJS_MATERIAL piece_23_solid_materials[1] = {
#include "cart/course_art/piece_23_solid_materials.inc"
};
static Sint16 piece_23_solid_indices[44] = {
#include "cart/course_art/piece_23_solid_indices.inc"
};
static NJS_MESHSET piece_23_solid_meshsets[1] = {
#include "cart/course_art/piece_23_solid_meshsets.inc"
};
static NJS_POINT3 piece_23_solid_points[25] = {
#include "cart/course_art/piece_23_solid_points.inc"
};
static NJS_POINT3 piece_23_solid_normals[25] = {
#include "cart/course_art/piece_23_solid_normals.inc"
};
static NJS_MODEL piece_23_solid_model = {
#include "cart/course_art/piece_23_solid_model.inc"
};
static NJS_OBJECT piece_23_solid = {
#include "cart/course_art/piece_23_solid.inc"
};
static NJS_MATERIAL piece_24_solid_materials[1] = {
#include "cart/course_art/piece_24_solid_materials.inc"
};
static Sint16 piece_24_solid_indices[44] = {
#include "cart/course_art/piece_24_solid_indices.inc"
};
static NJS_MESHSET piece_24_solid_meshsets[1] = {
#include "cart/course_art/piece_24_solid_meshsets.inc"
};
static NJS_POINT3 piece_24_solid_points[25] = {
#include "cart/course_art/piece_24_solid_points.inc"
};
static NJS_POINT3 piece_24_solid_normals[25] = {
#include "cart/course_art/piece_24_solid_normals.inc"
};
static NJS_MODEL piece_24_solid_model = {
#include "cart/course_art/piece_24_solid_model.inc"
};
static NJS_OBJECT piece_24_solid = {
#include "cart/course_art/piece_24_solid.inc"
};
static NJS_MATERIAL piece_28_solid_materials[1] = {
#include "cart/course_art/piece_28_solid_materials.inc"
};
static Sint16 piece_28_solid_indices[30] = {
#include "cart/course_art/piece_28_solid_indices.inc"
};
static NJS_MESHSET piece_28_solid_meshsets[1] = {
#include "cart/course_art/piece_28_solid_meshsets.inc"
};
static NJS_POINT3 piece_28_solid_points[17] = {
#include "cart/course_art/piece_28_solid_points.inc"
};
static NJS_POINT3 piece_28_solid_normals[17] = {
#include "cart/course_art/piece_28_solid_normals.inc"
};
static NJS_MODEL piece_28_solid_model = {
#include "cart/course_art/piece_28_solid_model.inc"
};
static NJS_OBJECT piece_28_solid = {
#include "cart/course_art/piece_28_solid.inc"
};
static NJS_MATERIAL piece_29_solid_materials[1] = {
#include "cart/course_art/piece_29_solid_materials.inc"
};
static Sint16 piece_29_solid_indices[30] = {
#include "cart/course_art/piece_29_solid_indices.inc"
};
static NJS_MESHSET piece_29_solid_meshsets[1] = {
#include "cart/course_art/piece_29_solid_meshsets.inc"
};
static NJS_POINT3 piece_29_solid_points[17] = {
#include "cart/course_art/piece_29_solid_points.inc"
};
static NJS_POINT3 piece_29_solid_normals[17] = {
#include "cart/course_art/piece_29_solid_normals.inc"
};
static NJS_MODEL piece_29_solid_model = {
#include "cart/course_art/piece_29_solid_model.inc"
};
static NJS_OBJECT piece_29_solid = {
#include "cart/course_art/piece_29_solid.inc"
};
static NJS_MATERIAL piece_30_solid_materials[1] = {
#include "cart/course_art/piece_30_solid_materials.inc"
};
static Sint16 piece_30_solid_indices[30] = {
#include "cart/course_art/piece_30_solid_indices.inc"
};
static NJS_MESHSET piece_30_solid_meshsets[1] = {
#include "cart/course_art/piece_30_solid_meshsets.inc"
};
static NJS_POINT3 piece_30_solid_points[17] = {
#include "cart/course_art/piece_30_solid_points.inc"
};
static NJS_POINT3 piece_30_solid_normals[17] = {
#include "cart/course_art/piece_30_solid_normals.inc"
};
static NJS_MODEL piece_30_solid_model = {
#include "cart/course_art/piece_30_solid_model.inc"
};
static NJS_OBJECT piece_30_solid = {
#include "cart/course_art/piece_30_solid.inc"
};
static NJS_MATERIAL piece_31_solid_materials[1] = {
#include "cart/course_art/piece_31_solid_materials.inc"
};
static Sint16 piece_31_solid_indices[30] = {
#include "cart/course_art/piece_31_solid_indices.inc"
};
static NJS_MESHSET piece_31_solid_meshsets[1] = {
#include "cart/course_art/piece_31_solid_meshsets.inc"
};
static NJS_POINT3 piece_31_solid_points[17] = {
#include "cart/course_art/piece_31_solid_points.inc"
};
static NJS_POINT3 piece_31_solid_normals[17] = {
#include "cart/course_art/piece_31_solid_normals.inc"
};
static NJS_MODEL piece_31_solid_model = {
#include "cart/course_art/piece_31_solid_model.inc"
};
static NJS_OBJECT piece_31_solid = {
#include "cart/course_art/piece_31_solid.inc"
};
static NJS_MATERIAL piece_32_solid_materials[1] = {
#include "cart/course_art/piece_32_solid_materials.inc"
};
static Sint16 piece_32_solid_indices[76] = {
#include "cart/course_art/piece_32_solid_indices.inc"
};
static NJS_MESHSET piece_32_solid_meshsets[1] = {
#include "cart/course_art/piece_32_solid_meshsets.inc"
};
static NJS_POINT3 piece_32_solid_points[45] = {
#include "cart/course_art/piece_32_solid_points.inc"
};
static NJS_POINT3 piece_32_solid_normals[45] = {
#include "cart/course_art/piece_32_solid_normals.inc"
};
static NJS_MODEL piece_32_solid_model = {
#include "cart/course_art/piece_32_solid_model.inc"
};
static NJS_OBJECT piece_32_solid = {
#include "cart/course_art/piece_32_solid.inc"
};
static NJS_MATERIAL piece_33_solid_materials[1] = {
#include "cart/course_art/piece_33_solid_materials.inc"
};
static Sint16 piece_33_solid_indices[76] = {
#include "cart/course_art/piece_33_solid_indices.inc"
};
static NJS_MESHSET piece_33_solid_meshsets[1] = {
#include "cart/course_art/piece_33_solid_meshsets.inc"
};
static NJS_POINT3 piece_33_solid_points[45] = {
#include "cart/course_art/piece_33_solid_points.inc"
};
static NJS_POINT3 piece_33_solid_normals[45] = {
#include "cart/course_art/piece_33_solid_normals.inc"
};
static NJS_MODEL piece_33_solid_model = {
#include "cart/course_art/piece_33_solid_model.inc"
};
static NJS_OBJECT piece_33_solid = {
#include "cart/course_art/piece_33_solid.inc"
};
static NJS_MATERIAL piece_34_solid_materials[1] = {
#include "cart/course_art/piece_34_solid_materials.inc"
};
static Sint16 piece_34_solid_indices[44] = {
#include "cart/course_art/piece_34_solid_indices.inc"
};
static NJS_MESHSET piece_34_solid_meshsets[1] = {
#include "cart/course_art/piece_34_solid_meshsets.inc"
};
static NJS_POINT3 piece_34_solid_points[25] = {
#include "cart/course_art/piece_34_solid_points.inc"
};
static NJS_POINT3 piece_34_solid_normals[25] = {
#include "cart/course_art/piece_34_solid_normals.inc"
};
static NJS_MODEL piece_34_solid_model = {
#include "cart/course_art/piece_34_solid_model.inc"
};
static NJS_OBJECT piece_34_solid = {
#include "cart/course_art/piece_34_solid.inc"
};
static NJS_MATERIAL piece_35_solid_materials[1] = {
#include "cart/course_art/piece_35_solid_materials.inc"
};
static Sint16 piece_35_solid_indices[44] = {
#include "cart/course_art/piece_35_solid_indices.inc"
};
static NJS_MESHSET piece_35_solid_meshsets[1] = {
#include "cart/course_art/piece_35_solid_meshsets.inc"
};
static NJS_POINT3 piece_35_solid_points[25] = {
#include "cart/course_art/piece_35_solid_points.inc"
};
static NJS_POINT3 piece_35_solid_normals[25] = {
#include "cart/course_art/piece_35_solid_normals.inc"
};
static NJS_MODEL piece_35_solid_model = {
#include "cart/course_art/piece_35_solid_model.inc"
};
static NJS_OBJECT piece_35_solid = {
#include "cart/course_art/piece_35_solid.inc"
};
static NJS_MATERIAL piece_36_solid_materials[1] = {
#include "cart/course_art/piece_36_solid_materials.inc"
};
static Sint16 piece_36_solid_indices[76] = {
#include "cart/course_art/piece_36_solid_indices.inc"
};
static NJS_MESHSET piece_36_solid_meshsets[1] = {
#include "cart/course_art/piece_36_solid_meshsets.inc"
};
static NJS_POINT3 piece_36_solid_points[45] = {
#include "cart/course_art/piece_36_solid_points.inc"
};
static NJS_POINT3 piece_36_solid_normals[45] = {
#include "cart/course_art/piece_36_solid_normals.inc"
};
static NJS_MODEL piece_36_solid_model = {
#include "cart/course_art/piece_36_solid_model.inc"
};
static NJS_OBJECT piece_36_solid = {
#include "cart/course_art/piece_36_solid.inc"
};
static NJS_MATERIAL piece_37_solid_materials[1] = {
#include "cart/course_art/piece_37_solid_materials.inc"
};
static Sint16 piece_37_solid_indices[76] = {
#include "cart/course_art/piece_37_solid_indices.inc"
};
static NJS_MESHSET piece_37_solid_meshsets[1] = {
#include "cart/course_art/piece_37_solid_meshsets.inc"
};
static NJS_POINT3 piece_37_solid_points[45] = {
#include "cart/course_art/piece_37_solid_points.inc"
};
static NJS_POINT3 piece_37_solid_normals[45] = {
#include "cart/course_art/piece_37_solid_normals.inc"
};
static NJS_MODEL piece_37_solid_model = {
#include "cart/course_art/piece_37_solid_model.inc"
};
static NJS_OBJECT piece_37_solid = {
#include "cart/course_art/piece_37_solid.inc"
};
static NJS_MATERIAL piece_38_solid_materials[1] = {
#include "cart/course_art/piece_38_solid_materials.inc"
};
static Sint16 piece_38_solid_indices[47] = {
#include "cart/course_art/piece_38_solid_indices.inc"
};
static NJS_MESHSET piece_38_solid_meshsets[1] = {
#include "cart/course_art/piece_38_solid_meshsets.inc"
};
static NJS_POINT3 piece_38_solid_points[25] = {
#include "cart/course_art/piece_38_solid_points.inc"
};
static NJS_POINT3 piece_38_solid_normals[25] = {
#include "cart/course_art/piece_38_solid_normals.inc"
};
static NJS_MODEL piece_38_solid_model = {
#include "cart/course_art/piece_38_solid_model.inc"
};
static NJS_OBJECT piece_38_solid = {
#include "cart/course_art/piece_38_solid.inc"
};
static NJS_MATERIAL piece_39_solid_materials[1] = {
#include "cart/course_art/piece_39_solid_materials.inc"
};
static Sint16 piece_39_solid_indices[44] = {
#include "cart/course_art/piece_39_solid_indices.inc"
};
static NJS_MESHSET piece_39_solid_meshsets[1] = {
#include "cart/course_art/piece_39_solid_meshsets.inc"
};
static NJS_POINT3 piece_39_solid_points[25] = {
#include "cart/course_art/piece_39_solid_points.inc"
};
static NJS_POINT3 piece_39_solid_normals[25] = {
#include "cart/course_art/piece_39_solid_normals.inc"
};
static NJS_MODEL piece_39_solid_model = {
#include "cart/course_art/piece_39_solid_model.inc"
};
static NJS_OBJECT piece_39_solid = {
#include "cart/course_art/piece_39_solid.inc"
};
static NJS_MATERIAL piece_40_solid_materials[1] = {
#include "cart/course_art/piece_40_solid_materials.inc"
};
static Sint16 piece_40_solid_indices[76] = {
#include "cart/course_art/piece_40_solid_indices.inc"
};
static NJS_MESHSET piece_40_solid_meshsets[1] = {
#include "cart/course_art/piece_40_solid_meshsets.inc"
};
static NJS_POINT3 piece_40_solid_points[45] = {
#include "cart/course_art/piece_40_solid_points.inc"
};
static NJS_POINT3 piece_40_solid_normals[45] = {
#include "cart/course_art/piece_40_solid_normals.inc"
};
static NJS_MODEL piece_40_solid_model = {
#include "cart/course_art/piece_40_solid_model.inc"
};
static NJS_OBJECT piece_40_solid = {
#include "cart/course_art/piece_40_solid.inc"
};
static NJS_MATERIAL piece_41_solid_materials[1] = {
#include "cart/course_art/piece_41_solid_materials.inc"
};
static Sint16 piece_41_solid_indices[76] = {
#include "cart/course_art/piece_41_solid_indices.inc"
};
static NJS_MESHSET piece_41_solid_meshsets[1] = {
#include "cart/course_art/piece_41_solid_meshsets.inc"
};
static NJS_POINT3 piece_41_solid_points[45] = {
#include "cart/course_art/piece_41_solid_points.inc"
};
static NJS_POINT3 piece_41_solid_normals[45] = {
#include "cart/course_art/piece_41_solid_normals.inc"
};
static NJS_MODEL piece_41_solid_model = {
#include "cart/course_art/piece_41_solid_model.inc"
};
static NJS_OBJECT piece_41_solid = {
#include "cart/course_art/piece_41_solid.inc"
};
static NJS_MATERIAL piece_42_solid_materials[1] = {
#include "cart/course_art/piece_42_solid_materials.inc"
};
static Sint16 piece_42_solid_indices[44] = {
#include "cart/course_art/piece_42_solid_indices.inc"
};
static NJS_MESHSET piece_42_solid_meshsets[1] = {
#include "cart/course_art/piece_42_solid_meshsets.inc"
};
static NJS_POINT3 piece_42_solid_points[25] = {
#include "cart/course_art/piece_42_solid_points.inc"
};
static NJS_POINT3 piece_42_solid_normals[25] = {
#include "cart/course_art/piece_42_solid_normals.inc"
};
static NJS_MODEL piece_42_solid_model = {
#include "cart/course_art/piece_42_solid_model.inc"
};
static NJS_OBJECT piece_42_solid = {
#include "cart/course_art/piece_42_solid.inc"
};
static NJS_MATERIAL piece_43_solid_materials[1] = {
#include "cart/course_art/piece_43_solid_materials.inc"
};
static Sint16 piece_43_solid_indices[44] = {
#include "cart/course_art/piece_43_solid_indices.inc"
};
static NJS_MESHSET piece_43_solid_meshsets[1] = {
#include "cart/course_art/piece_43_solid_meshsets.inc"
};
static NJS_POINT3 piece_43_solid_points[25] = {
#include "cart/course_art/piece_43_solid_points.inc"
};
static NJS_POINT3 piece_43_solid_normals[25] = {
#include "cart/course_art/piece_43_solid_normals.inc"
};
static NJS_MODEL piece_43_solid_model = {
#include "cart/course_art/piece_43_solid_model.inc"
};
static NJS_OBJECT piece_43_solid = {
#include "cart/course_art/piece_43_solid.inc"
};
static NJS_MATERIAL piece_44_solid_materials[1] = {
#include "cart/course_art/piece_44_solid_materials.inc"
};
static Sint16 piece_44_solid_indices[86] = {
#include "cart/course_art/piece_44_solid_indices.inc"
};
static NJS_MESHSET piece_44_solid_meshsets[1] = {
#include "cart/course_art/piece_44_solid_meshsets.inc"
};
static NJS_POINT3 piece_44_solid_points[37] = {
#include "cart/course_art/piece_44_solid_points.inc"
};
static NJS_POINT3 piece_44_solid_normals[37] = {
#include "cart/course_art/piece_44_solid_normals.inc"
};
static NJS_MODEL piece_44_solid_model = {
#include "cart/course_art/piece_44_solid_model.inc"
};
static NJS_OBJECT piece_44_solid = {
#include "cart/course_art/piece_44_solid.inc"
};
static NJS_MATERIAL piece_45_solid_materials[1] = {
#include "cart/course_art/piece_45_solid_materials.inc"
};
static Sint16 piece_45_solid_indices[83] = {
#include "cart/course_art/piece_45_solid_indices.inc"
};
static NJS_MESHSET piece_45_solid_meshsets[1] = {
#include "cart/course_art/piece_45_solid_meshsets.inc"
};
static NJS_POINT3 piece_45_solid_points[37] = {
#include "cart/course_art/piece_45_solid_points.inc"
};
static NJS_POINT3 piece_45_solid_normals[37] = {
#include "cart/course_art/piece_45_solid_normals.inc"
};
static NJS_MODEL piece_45_solid_model = {
#include "cart/course_art/piece_45_solid_model.inc"
};
static NJS_OBJECT piece_45_solid = {
#include "cart/course_art/piece_45_solid.inc"
};
static NJS_MATERIAL piece_46_solid_materials[1] = {
#include "cart/course_art/piece_46_solid_materials.inc"
};
static Sint16 piece_46_solid_indices[89] = {
#include "cart/course_art/piece_46_solid_indices.inc"
};
static NJS_MESHSET piece_46_solid_meshsets[1] = {
#include "cart/course_art/piece_46_solid_meshsets.inc"
};
static NJS_POINT3 piece_46_solid_points[38] = {
#include "cart/course_art/piece_46_solid_points.inc"
};
static NJS_POINT3 piece_46_solid_normals[38] = {
#include "cart/course_art/piece_46_solid_normals.inc"
};
static NJS_MODEL piece_46_solid_model = {
#include "cart/course_art/piece_46_solid_model.inc"
};
static NJS_OBJECT piece_46_solid = {
#include "cart/course_art/piece_46_solid.inc"
};
static NJS_MATERIAL piece_47_solid_materials[1] = {
#include "cart/course_art/piece_47_solid_materials.inc"
};
static Sint16 piece_47_solid_indices[83] = {
#include "cart/course_art/piece_47_solid_indices.inc"
};
static NJS_MESHSET piece_47_solid_meshsets[1] = {
#include "cart/course_art/piece_47_solid_meshsets.inc"
};
static NJS_POINT3 piece_47_solid_points[38] = {
#include "cart/course_art/piece_47_solid_points.inc"
};
static NJS_POINT3 piece_47_solid_normals[38] = {
#include "cart/course_art/piece_47_solid_normals.inc"
};
static NJS_MODEL piece_47_solid_model = {
#include "cart/course_art/piece_47_solid_model.inc"
};
static NJS_OBJECT piece_47_solid = {
#include "cart/course_art/piece_47_solid.inc"
};
static NJS_MATERIAL piece_48_solid_materials[1] = {
#include "cart/course_art/piece_48_solid_materials.inc"
};
static Sint16 piece_48_solid_indices[89] = {
#include "cart/course_art/piece_48_solid_indices.inc"
};
static NJS_MESHSET piece_48_solid_meshsets[1] = {
#include "cart/course_art/piece_48_solid_meshsets.inc"
};
static NJS_POINT3 piece_48_solid_points[40] = {
#include "cart/course_art/piece_48_solid_points.inc"
};
static NJS_POINT3 piece_48_solid_normals[40] = {
#include "cart/course_art/piece_48_solid_normals.inc"
};
static NJS_MODEL piece_48_solid_model = {
#include "cart/course_art/piece_48_solid_model.inc"
};
static NJS_OBJECT piece_48_solid = {
#include "cart/course_art/piece_48_solid.inc"
};
static NJS_MATERIAL piece_49_solid_materials[1] = {
#include "cart/course_art/piece_49_solid_materials.inc"
};
static Sint16 piece_49_solid_indices[89] = {
#include "cart/course_art/piece_49_solid_indices.inc"
};
static NJS_MESHSET piece_49_solid_meshsets[1] = {
#include "cart/course_art/piece_49_solid_meshsets.inc"
};
static NJS_POINT3 piece_49_solid_points[40] = {
#include "cart/course_art/piece_49_solid_points.inc"
};
static NJS_POINT3 piece_49_solid_normals[40] = {
#include "cart/course_art/piece_49_solid_normals.inc"
};
static NJS_MODEL piece_49_solid_model = {
#include "cart/course_art/piece_49_solid_model.inc"
};
static NJS_OBJECT piece_49_solid = {
#include "cart/course_art/piece_49_solid.inc"
};
static NJS_MATERIAL piece_50_solid_materials[1] = {
#include "cart/course_art/piece_50_solid_materials.inc"
};
static Sint16 piece_50_solid_indices[71] = {
#include "cart/course_art/piece_50_solid_indices.inc"
};
static NJS_MESHSET piece_50_solid_meshsets[1] = {
#include "cart/course_art/piece_50_solid_meshsets.inc"
};
static NJS_POINT3 piece_50_solid_points[38] = {
#include "cart/course_art/piece_50_solid_points.inc"
};
static NJS_POINT3 piece_50_solid_normals[38] = {
#include "cart/course_art/piece_50_solid_normals.inc"
};
static NJS_MODEL piece_50_solid_model = {
#include "cart/course_art/piece_50_solid_model.inc"
};
static NJS_OBJECT piece_50_solid = {
#include "cart/course_art/piece_50_solid.inc"
};
static NJS_MATERIAL piece_51_solid_materials[1] = {
#include "cart/course_art/piece_51_solid_materials.inc"
};
static Sint16 piece_51_solid_indices[71] = {
#include "cart/course_art/piece_51_solid_indices.inc"
};
static NJS_MESHSET piece_51_solid_meshsets[1] = {
#include "cart/course_art/piece_51_solid_meshsets.inc"
};
static NJS_POINT3 piece_51_solid_points[38] = {
#include "cart/course_art/piece_51_solid_points.inc"
};
static NJS_POINT3 piece_51_solid_normals[38] = {
#include "cart/course_art/piece_51_solid_normals.inc"
};
static NJS_MODEL piece_51_solid_model = {
#include "cart/course_art/piece_51_solid_model.inc"
};
static NJS_OBJECT piece_51_solid = {
#include "cart/course_art/piece_51_solid.inc"
};
static NJS_MATERIAL piece_52_solid_materials[1] = {
#include "cart/course_art/piece_52_solid_materials.inc"
};
static Sint16 piece_52_solid_indices[69] = {
#include "cart/course_art/piece_52_solid_indices.inc"
};
static NJS_MESHSET piece_52_solid_meshsets[1] = {
#include "cart/course_art/piece_52_solid_meshsets.inc"
};
static NJS_POINT3 piece_52_solid_points[38] = {
#include "cart/course_art/piece_52_solid_points.inc"
};
static NJS_POINT3 piece_52_solid_normals[38] = {
#include "cart/course_art/piece_52_solid_normals.inc"
};
static NJS_MODEL piece_52_solid_model = {
#include "cart/course_art/piece_52_solid_model.inc"
};
static NJS_OBJECT piece_52_solid = {
#include "cart/course_art/piece_52_solid.inc"
};
static NJS_MATERIAL piece_53_solid_materials[1] = {
#include "cart/course_art/piece_53_solid_materials.inc"
};
static Sint16 piece_53_solid_indices[102] = {
#include "cart/course_art/piece_53_solid_indices.inc"
};
static NJS_MESHSET piece_53_solid_meshsets[1] = {
#include "cart/course_art/piece_53_solid_meshsets.inc"
};
static NJS_POINT3 piece_53_solid_points[45] = {
#include "cart/course_art/piece_53_solid_points.inc"
};
static NJS_POINT3 piece_53_solid_normals[45] = {
#include "cart/course_art/piece_53_solid_normals.inc"
};
static NJS_MODEL piece_53_solid_model = {
#include "cart/course_art/piece_53_solid_model.inc"
};
static NJS_OBJECT piece_53_solid = {
#include "cart/course_art/piece_53_solid.inc"
};
static NJS_MATERIAL piece_54_solid_materials[1] = {
#include "cart/course_art/piece_54_solid_materials.inc"
};
static Sint16 piece_54_solid_indices[102] = {
#include "cart/course_art/piece_54_solid_indices.inc"
};
static NJS_MESHSET piece_54_solid_meshsets[1] = {
#include "cart/course_art/piece_54_solid_meshsets.inc"
};
static NJS_POINT3 piece_54_solid_points[45] = {
#include "cart/course_art/piece_54_solid_points.inc"
};
static NJS_POINT3 piece_54_solid_normals[45] = {
#include "cart/course_art/piece_54_solid_normals.inc"
};
static NJS_MODEL piece_54_solid_model = {
#include "cart/course_art/piece_54_solid_model.inc"
};
static NJS_OBJECT piece_54_solid = {
#include "cart/course_art/piece_54_solid.inc"
};
static NJS_MATERIAL piece_55_solid_materials[1] = {
#include "cart/course_art/piece_55_solid_materials.inc"
};
static Sint16 piece_55_solid_indices[63] = {
#include "cart/course_art/piece_55_solid_indices.inc"
};
static NJS_MESHSET piece_55_solid_meshsets[1] = {
#include "cart/course_art/piece_55_solid_meshsets.inc"
};
static NJS_POINT3 piece_55_solid_points[36] = {
#include "cart/course_art/piece_55_solid_points.inc"
};
static NJS_POINT3 piece_55_solid_normals[36] = {
#include "cart/course_art/piece_55_solid_normals.inc"
};
static NJS_MODEL piece_55_solid_model = {
#include "cart/course_art/piece_55_solid_model.inc"
};
static NJS_OBJECT piece_55_solid = {
#include "cart/course_art/piece_55_solid.inc"
};
static NJS_MATERIAL piece_56_solid_materials[1] = {
#include "cart/course_art/piece_56_solid_materials.inc"
};
static Sint16 piece_56_solid_indices[63] = {
#include "cart/course_art/piece_56_solid_indices.inc"
};
static NJS_MESHSET piece_56_solid_meshsets[1] = {
#include "cart/course_art/piece_56_solid_meshsets.inc"
};
static NJS_POINT3 piece_56_solid_points[36] = {
#include "cart/course_art/piece_56_solid_points.inc"
};
static NJS_POINT3 piece_56_solid_normals[36] = {
#include "cart/course_art/piece_56_solid_normals.inc"
};
static NJS_MODEL piece_56_solid_model = {
#include "cart/course_art/piece_56_solid_model.inc"
};
static NJS_OBJECT piece_56_solid = {
#include "cart/course_art/piece_56_solid.inc"
};
static NJS_MATERIAL piece_57_solid_materials[1] = {
#include "cart/course_art/piece_57_solid_materials.inc"
};
static Sint16 piece_57_solid_indices[72] = {
#include "cart/course_art/piece_57_solid_indices.inc"
};
static NJS_MESHSET piece_57_solid_meshsets[1] = {
#include "cart/course_art/piece_57_solid_meshsets.inc"
};
static NJS_POINT3 piece_57_solid_points[32] = {
#include "cart/course_art/piece_57_solid_points.inc"
};
static NJS_POINT3 piece_57_solid_normals[32] = {
#include "cart/course_art/piece_57_solid_normals.inc"
};
static NJS_MODEL piece_57_solid_model = {
#include "cart/course_art/piece_57_solid_model.inc"
};
static NJS_OBJECT piece_57_solid = {
#include "cart/course_art/piece_57_solid.inc"
};
static NJS_MATERIAL piece_58_solid_materials[1] = {
#include "cart/course_art/piece_58_solid_materials.inc"
};
static Sint16 piece_58_solid_indices[84] = {
#include "cart/course_art/piece_58_solid_indices.inc"
};
static NJS_MESHSET piece_58_solid_meshsets[1] = {
#include "cart/course_art/piece_58_solid_meshsets.inc"
};
static NJS_POINT3 piece_58_solid_points[45] = {
#include "cart/course_art/piece_58_solid_points.inc"
};
static NJS_POINT3 piece_58_solid_normals[45] = {
#include "cart/course_art/piece_58_solid_normals.inc"
};
static NJS_MODEL piece_58_solid_model = {
#include "cart/course_art/piece_58_solid_model.inc"
};
static NJS_OBJECT piece_58_solid = {
#include "cart/course_art/piece_58_solid.inc"
};
static NJS_MATERIAL piece_59_solid_materials[1] = {
#include "cart/course_art/piece_59_solid_materials.inc"
};
static Sint16 piece_59_solid_indices[84] = {
#include "cart/course_art/piece_59_solid_indices.inc"
};
static NJS_MESHSET piece_59_solid_meshsets[1] = {
#include "cart/course_art/piece_59_solid_meshsets.inc"
};
static NJS_POINT3 piece_59_solid_points[45] = {
#include "cart/course_art/piece_59_solid_points.inc"
};
static NJS_POINT3 piece_59_solid_normals[45] = {
#include "cart/course_art/piece_59_solid_normals.inc"
};
static NJS_MODEL piece_59_solid_model = {
#include "cart/course_art/piece_59_solid_model.inc"
};
static NJS_OBJECT piece_59_solid = {
#include "cart/course_art/piece_59_solid.inc"
};
static NJS_MATERIAL piece_60_solid_materials[1] = {
#include "cart/course_art/piece_60_solid_materials.inc"
};
static Sint16 piece_60_solid_indices[84] = {
#include "cart/course_art/piece_60_solid_indices.inc"
};
static NJS_MESHSET piece_60_solid_meshsets[1] = {
#include "cart/course_art/piece_60_solid_meshsets.inc"
};
static NJS_POINT3 piece_60_solid_points[45] = {
#include "cart/course_art/piece_60_solid_points.inc"
};
static NJS_POINT3 piece_60_solid_normals[45] = {
#include "cart/course_art/piece_60_solid_normals.inc"
};
static NJS_MODEL piece_60_solid_model = {
#include "cart/course_art/piece_60_solid_model.inc"
};
static NJS_OBJECT piece_60_solid = {
#include "cart/course_art/piece_60_solid.inc"
};
static NJS_MATERIAL piece_61_solid_materials[1] = {
#include "cart/course_art/piece_61_solid_materials.inc"
};
static Sint16 piece_61_solid_indices[46] = {
#include "cart/course_art/piece_61_solid_indices.inc"
};
static NJS_MESHSET piece_61_solid_meshsets[1] = {
#include "cart/course_art/piece_61_solid_meshsets.inc"
};
static NJS_POINT3 piece_61_solid_points[28] = {
#include "cart/course_art/piece_61_solid_points.inc"
};
static NJS_POINT3 piece_61_solid_normals[28] = {
#include "cart/course_art/piece_61_solid_normals.inc"
};
static NJS_MODEL piece_61_solid_model = {
#include "cart/course_art/piece_61_solid_model.inc"
};
static NJS_OBJECT piece_61_solid = {
#include "cart/course_art/piece_61_solid.inc"
};
static NJS_MATERIAL piece_62_solid_materials[1] = {
#include "cart/course_art/piece_62_solid_materials.inc"
};
static Sint16 piece_62_solid_indices[84] = {
#include "cart/course_art/piece_62_solid_indices.inc"
};
static NJS_MESHSET piece_62_solid_meshsets[1] = {
#include "cart/course_art/piece_62_solid_meshsets.inc"
};
static NJS_POINT3 piece_62_solid_points[45] = {
#include "cart/course_art/piece_62_solid_points.inc"
};
static NJS_POINT3 piece_62_solid_normals[45] = {
#include "cart/course_art/piece_62_solid_normals.inc"
};
static NJS_MODEL piece_62_solid_model = {
#include "cart/course_art/piece_62_solid_model.inc"
};
static NJS_OBJECT piece_62_solid = {
#include "cart/course_art/piece_62_solid.inc"
};
static NJS_MATERIAL piece_63_solid_materials[1] = {
#include "cart/course_art/piece_63_solid_materials.inc"
};
static Sint16 piece_63_solid_indices[84] = {
#include "cart/course_art/piece_63_solid_indices.inc"
};
static NJS_MESHSET piece_63_solid_meshsets[1] = {
#include "cart/course_art/piece_63_solid_meshsets.inc"
};
static NJS_POINT3 piece_63_solid_points[45] = {
#include "cart/course_art/piece_63_solid_points.inc"
};
static NJS_POINT3 piece_63_solid_normals[45] = {
#include "cart/course_art/piece_63_solid_normals.inc"
};
static NJS_MODEL piece_63_solid_model = {
#include "cart/course_art/piece_63_solid_model.inc"
};
static NJS_OBJECT piece_63_solid = {
#include "cart/course_art/piece_63_solid.inc"
};
static NJS_MATERIAL piece_64_solid_materials[1] = {
#include "cart/course_art/piece_64_solid_materials.inc"
};
static Sint16 piece_64_solid_indices[46] = {
#include "cart/course_art/piece_64_solid_indices.inc"
};
static NJS_MESHSET piece_64_solid_meshsets[1] = {
#include "cart/course_art/piece_64_solid_meshsets.inc"
};
static NJS_POINT3 piece_64_solid_points[28] = {
#include "cart/course_art/piece_64_solid_points.inc"
};
static NJS_POINT3 piece_64_solid_normals[28] = {
#include "cart/course_art/piece_64_solid_normals.inc"
};
static NJS_MODEL piece_64_solid_model = {
#include "cart/course_art/piece_64_solid_model.inc"
};
static NJS_OBJECT piece_64_solid = {
#include "cart/course_art/piece_64_solid.inc"
};
static NJS_MATERIAL piece_65_solid_materials[1] = {
#include "cart/course_art/piece_65_solid_materials.inc"
};
static Sint16 piece_65_solid_indices[84] = {
#include "cart/course_art/piece_65_solid_indices.inc"
};
static NJS_MESHSET piece_65_solid_meshsets[1] = {
#include "cart/course_art/piece_65_solid_meshsets.inc"
};
static NJS_POINT3 piece_65_solid_points[45] = {
#include "cart/course_art/piece_65_solid_points.inc"
};
static NJS_POINT3 piece_65_solid_normals[45] = {
#include "cart/course_art/piece_65_solid_normals.inc"
};
static NJS_MODEL piece_65_solid_model = {
#include "cart/course_art/piece_65_solid_model.inc"
};
static NJS_OBJECT piece_65_solid = {
#include "cart/course_art/piece_65_solid.inc"
};
static NJS_MATERIAL piece_66_solid_materials[1] = {
#include "cart/course_art/piece_66_solid_materials.inc"
};
static Sint16 piece_66_solid_indices[84] = {
#include "cart/course_art/piece_66_solid_indices.inc"
};
static NJS_MESHSET piece_66_solid_meshsets[1] = {
#include "cart/course_art/piece_66_solid_meshsets.inc"
};
static NJS_POINT3 piece_66_solid_points[45] = {
#include "cart/course_art/piece_66_solid_points.inc"
};
static NJS_POINT3 piece_66_solid_normals[45] = {
#include "cart/course_art/piece_66_solid_normals.inc"
};
static NJS_MODEL piece_66_solid_model = {
#include "cart/course_art/piece_66_solid_model.inc"
};
static NJS_OBJECT piece_66_solid = {
#include "cart/course_art/piece_66_solid.inc"
};
static NJS_MATERIAL piece_67_solid_materials[1] = {
#include "cart/course_art/piece_67_solid_materials.inc"
};
static Sint16 piece_67_solid_indices[46] = {
#include "cart/course_art/piece_67_solid_indices.inc"
};
static NJS_MESHSET piece_67_solid_meshsets[1] = {
#include "cart/course_art/piece_67_solid_meshsets.inc"
};
static NJS_POINT3 piece_67_solid_points[28] = {
#include "cart/course_art/piece_67_solid_points.inc"
};
static NJS_POINT3 piece_67_solid_normals[28] = {
#include "cart/course_art/piece_67_solid_normals.inc"
};
static NJS_MODEL piece_67_solid_model = {
#include "cart/course_art/piece_67_solid_model.inc"
};
static NJS_OBJECT piece_67_solid = {
#include "cart/course_art/piece_67_solid.inc"
};
static NJS_MATERIAL piece_68_solid_materials[1] = {
#include "cart/course_art/piece_68_solid_materials.inc"
};
static Sint16 piece_68_solid_indices[84] = {
#include "cart/course_art/piece_68_solid_indices.inc"
};
static NJS_MESHSET piece_68_solid_meshsets[1] = {
#include "cart/course_art/piece_68_solid_meshsets.inc"
};
static NJS_POINT3 piece_68_solid_points[45] = {
#include "cart/course_art/piece_68_solid_points.inc"
};
static NJS_POINT3 piece_68_solid_normals[45] = {
#include "cart/course_art/piece_68_solid_normals.inc"
};
static NJS_MODEL piece_68_solid_model = {
#include "cart/course_art/piece_68_solid_model.inc"
};
static NJS_OBJECT piece_68_solid = {
#include "cart/course_art/piece_68_solid.inc"
};
static NJS_MATERIAL piece_69_solid_materials[1] = {
#include "cart/course_art/piece_69_solid_materials.inc"
};
static Sint16 piece_69_solid_indices[84] = {
#include "cart/course_art/piece_69_solid_indices.inc"
};
static NJS_MESHSET piece_69_solid_meshsets[1] = {
#include "cart/course_art/piece_69_solid_meshsets.inc"
};
static NJS_POINT3 piece_69_solid_points[45] = {
#include "cart/course_art/piece_69_solid_points.inc"
};
static NJS_POINT3 piece_69_solid_normals[45] = {
#include "cart/course_art/piece_69_solid_normals.inc"
};
static NJS_MODEL piece_69_solid_model = {
#include "cart/course_art/piece_69_solid_model.inc"
};
static NJS_OBJECT piece_69_solid = {
#include "cart/course_art/piece_69_solid.inc"
};
static NJS_MATERIAL piece_70_solid_materials[1] = {
#include "cart/course_art/piece_70_solid_materials.inc"
};
static Sint16 piece_70_solid_indices[46] = {
#include "cart/course_art/piece_70_solid_indices.inc"
};
static NJS_MESHSET piece_70_solid_meshsets[1] = {
#include "cart/course_art/piece_70_solid_meshsets.inc"
};
static NJS_POINT3 piece_70_solid_points[28] = {
#include "cart/course_art/piece_70_solid_points.inc"
};
static NJS_POINT3 piece_70_solid_normals[28] = {
#include "cart/course_art/piece_70_solid_normals.inc"
};
static NJS_MODEL piece_70_solid_model = {
#include "cart/course_art/piece_70_solid_model.inc"
};
static NJS_OBJECT piece_70_solid = {
#include "cart/course_art/piece_70_solid.inc"
};
static NJS_MATERIAL piece_71_solid_materials[1] = {
#include "cart/course_art/piece_71_solid_materials.inc"
};
static Sint16 piece_71_solid_indices[84] = {
#include "cart/course_art/piece_71_solid_indices.inc"
};
static NJS_MESHSET piece_71_solid_meshsets[1] = {
#include "cart/course_art/piece_71_solid_meshsets.inc"
};
static NJS_POINT3 piece_71_solid_points[45] = {
#include "cart/course_art/piece_71_solid_points.inc"
};
static NJS_POINT3 piece_71_solid_normals[45] = {
#include "cart/course_art/piece_71_solid_normals.inc"
};
static NJS_MODEL piece_71_solid_model = {
#include "cart/course_art/piece_71_solid_model.inc"
};
static NJS_OBJECT piece_71_solid = {
#include "cart/course_art/piece_71_solid.inc"
};
static NJS_MATERIAL piece_72_solid_materials[1] = {
#include "cart/course_art/piece_72_solid_materials.inc"
};
static Sint16 piece_72_solid_indices[216] = {
#include "cart/course_art/piece_72_solid_indices.inc"
};
static NJS_MESHSET piece_72_solid_meshsets[1] = {
#include "cart/course_art/piece_72_solid_meshsets.inc"
};
static NJS_POINT3 piece_72_solid_points[117] = {
#include "cart/course_art/piece_72_solid_points.inc"
};
static NJS_POINT3 piece_72_solid_normals[117] = {
#include "cart/course_art/piece_72_solid_normals.inc"
};
static NJS_MODEL piece_72_solid_model = {
#include "cart/course_art/piece_72_solid_model.inc"
};
static NJS_OBJECT piece_72_solid = {
#include "cart/course_art/piece_72_solid.inc"
};
static NJS_MATERIAL piece_73_solid_materials[1] = {
#include "cart/course_art/piece_73_solid_materials.inc"
};
static Sint16 piece_73_solid_indices[114] = {
#include "cart/course_art/piece_73_solid_indices.inc"
};
static NJS_MESHSET piece_73_solid_meshsets[1] = {
#include "cart/course_art/piece_73_solid_meshsets.inc"
};
static NJS_POINT3 piece_73_solid_points[63] = {
#include "cart/course_art/piece_73_solid_points.inc"
};
static NJS_POINT3 piece_73_solid_normals[63] = {
#include "cart/course_art/piece_73_solid_normals.inc"
};
static NJS_MODEL piece_73_solid_model = {
#include "cart/course_art/piece_73_solid_model.inc"
};
static NJS_OBJECT piece_73_solid = {
#include "cart/course_art/piece_73_solid.inc"
};
static NJS_MATERIAL piece_74_solid_materials[1] = {
#include "cart/course_art/piece_74_solid_materials.inc"
};
static Sint16 piece_74_solid_indices[216] = {
#include "cart/course_art/piece_74_solid_indices.inc"
};
static NJS_MESHSET piece_74_solid_meshsets[1] = {
#include "cart/course_art/piece_74_solid_meshsets.inc"
};
static NJS_POINT3 piece_74_solid_points[117] = {
#include "cart/course_art/piece_74_solid_points.inc"
};
static NJS_POINT3 piece_74_solid_normals[117] = {
#include "cart/course_art/piece_74_solid_normals.inc"
};
static NJS_MODEL piece_74_solid_model = {
#include "cart/course_art/piece_74_solid_model.inc"
};
static NJS_OBJECT piece_74_solid = {
#include "cart/course_art/piece_74_solid.inc"
};
static NJS_MATERIAL piece_75_solid_materials[1] = {
#include "cart/course_art/piece_75_solid_materials.inc"
};
static Sint16 piece_75_solid_indices[216] = {
#include "cart/course_art/piece_75_solid_indices.inc"
};
static NJS_MESHSET piece_75_solid_meshsets[1] = {
#include "cart/course_art/piece_75_solid_meshsets.inc"
};
static NJS_POINT3 piece_75_solid_points[117] = {
#include "cart/course_art/piece_75_solid_points.inc"
};
static NJS_POINT3 piece_75_solid_normals[117] = {
#include "cart/course_art/piece_75_solid_normals.inc"
};
static NJS_MODEL piece_75_solid_model = {
#include "cart/course_art/piece_75_solid_model.inc"
};
static NJS_OBJECT piece_75_solid = {
#include "cart/course_art/piece_75_solid.inc"
};
static NJS_MATERIAL piece_76_solid_materials[1] = {
#include "cart/course_art/piece_76_solid_materials.inc"
};
static Sint16 piece_76_solid_indices[114] = {
#include "cart/course_art/piece_76_solid_indices.inc"
};
static NJS_MESHSET piece_76_solid_meshsets[1] = {
#include "cart/course_art/piece_76_solid_meshsets.inc"
};
static NJS_POINT3 piece_76_solid_points[63] = {
#include "cart/course_art/piece_76_solid_points.inc"
};
static NJS_POINT3 piece_76_solid_normals[63] = {
#include "cart/course_art/piece_76_solid_normals.inc"
};
static NJS_MODEL piece_76_solid_model = {
#include "cart/course_art/piece_76_solid_model.inc"
};
static NJS_OBJECT piece_76_solid = {
#include "cart/course_art/piece_76_solid.inc"
};
static NJS_MATERIAL piece_77_solid_materials[1] = {
#include "cart/course_art/piece_77_solid_materials.inc"
};
static Sint16 piece_77_solid_indices[216] = {
#include "cart/course_art/piece_77_solid_indices.inc"
};
static NJS_MESHSET piece_77_solid_meshsets[1] = {
#include "cart/course_art/piece_77_solid_meshsets.inc"
};
static NJS_POINT3 piece_77_solid_points[117] = {
#include "cart/course_art/piece_77_solid_points.inc"
};
static NJS_POINT3 piece_77_solid_normals[117] = {
#include "cart/course_art/piece_77_solid_normals.inc"
};
static NJS_MODEL piece_77_solid_model = {
#include "cart/course_art/piece_77_solid_model.inc"
};
static NJS_OBJECT piece_77_solid = {
#include "cart/course_art/piece_77_solid.inc"
};
static NJS_MATERIAL piece_78_solid_materials[1] = {
#include "cart/course_art/piece_78_solid_materials.inc"
};
static Sint16 piece_78_solid_indices[30] = {
#include "cart/course_art/piece_78_solid_indices.inc"
};
static NJS_MESHSET piece_78_solid_meshsets[1] = {
#include "cart/course_art/piece_78_solid_meshsets.inc"
};
static NJS_POINT3 piece_78_solid_points[21] = {
#include "cart/course_art/piece_78_solid_points.inc"
};
static NJS_POINT3 piece_78_solid_normals[21] = {
#include "cart/course_art/piece_78_solid_normals.inc"
};
static NJS_MODEL piece_78_solid_model = {
#include "cart/course_art/piece_78_solid_model.inc"
};
static NJS_OBJECT piece_78_solid = {
#include "cart/course_art/piece_78_solid.inc"
};
static NJS_MATERIAL piece_79_solid_materials[1] = {
#include "cart/course_art/piece_79_solid_materials.inc"
};
static Sint16 piece_79_solid_indices[30] = {
#include "cart/course_art/piece_79_solid_indices.inc"
};
static NJS_MESHSET piece_79_solid_meshsets[1] = {
#include "cart/course_art/piece_79_solid_meshsets.inc"
};
static NJS_POINT3 piece_79_solid_points[21] = {
#include "cart/course_art/piece_79_solid_points.inc"
};
static NJS_POINT3 piece_79_solid_normals[21] = {
#include "cart/course_art/piece_79_solid_normals.inc"
};
static NJS_MODEL piece_79_solid_model = {
#include "cart/course_art/piece_79_solid_model.inc"
};
static NJS_OBJECT piece_79_solid = {
#include "cart/course_art/piece_79_solid.inc"
};
static NJS_MATERIAL piece_80_solid_materials[1] = {
#include "cart/course_art/piece_80_solid_materials.inc"
};
static Sint16 piece_80_solid_indices[92] = {
#include "cart/course_art/piece_80_solid_indices.inc"
};
static NJS_MESHSET piece_80_solid_meshsets[1] = {
#include "cart/course_art/piece_80_solid_meshsets.inc"
};
static NJS_POINT3 piece_80_solid_points[55] = {
#include "cart/course_art/piece_80_solid_points.inc"
};
static NJS_POINT3 piece_80_solid_normals[55] = {
#include "cart/course_art/piece_80_solid_normals.inc"
};
static NJS_MODEL piece_80_solid_model = {
#include "cart/course_art/piece_80_solid_model.inc"
};
static NJS_OBJECT piece_80_solid = {
#include "cart/course_art/piece_80_solid.inc"
};
static NJS_MATERIAL piece_81_solid_materials[1] = {
#include "cart/course_art/piece_81_solid_materials.inc"
};
static Sint16 piece_81_solid_indices[14] = {
#include "cart/course_art/piece_81_solid_indices.inc"
};
static NJS_MESHSET piece_81_solid_meshsets[1] = {
#include "cart/course_art/piece_81_solid_meshsets.inc"
};
static NJS_POINT3 piece_81_solid_points[10] = {
#include "cart/course_art/piece_81_solid_points.inc"
};
static NJS_POINT3 piece_81_solid_normals[10] = {
#include "cart/course_art/piece_81_solid_normals.inc"
};
static NJS_MODEL piece_81_solid_model = {
#include "cart/course_art/piece_81_solid_model.inc"
};
static NJS_OBJECT piece_81_solid = {
#include "cart/course_art/piece_81_solid.inc"
};
static NJS_MATERIAL piece_82_solid_materials[1] = {
#include "cart/course_art/piece_82_solid_materials.inc"
};
static Sint16 piece_82_solid_indices[14] = {
#include "cart/course_art/piece_82_solid_indices.inc"
};
static NJS_MESHSET piece_82_solid_meshsets[1] = {
#include "cart/course_art/piece_82_solid_meshsets.inc"
};
static NJS_POINT3 piece_82_solid_points[10] = {
#include "cart/course_art/piece_82_solid_points.inc"
};
static NJS_POINT3 piece_82_solid_normals[10] = {
#include "cart/course_art/piece_82_solid_normals.inc"
};
static NJS_MODEL piece_82_solid_model = {
#include "cart/course_art/piece_82_solid_model.inc"
};
static NJS_OBJECT piece_82_solid = {
#include "cart/course_art/piece_82_solid.inc"
};
static NJS_MATERIAL piece_83_solid_materials[1] = {
#include "cart/course_art/piece_83_solid_materials.inc"
};
static Sint16 piece_83_solid_indices[14] = {
#include "cart/course_art/piece_83_solid_indices.inc"
};
static NJS_MESHSET piece_83_solid_meshsets[1] = {
#include "cart/course_art/piece_83_solid_meshsets.inc"
};
static NJS_POINT3 piece_83_solid_points[10] = {
#include "cart/course_art/piece_83_solid_points.inc"
};
static NJS_POINT3 piece_83_solid_normals[10] = {
#include "cart/course_art/piece_83_solid_normals.inc"
};
static NJS_MODEL piece_83_solid_model = {
#include "cart/course_art/piece_83_solid_model.inc"
};
static NJS_OBJECT piece_83_solid = {
#include "cart/course_art/piece_83_solid.inc"
};
static NJS_MATERIAL piece_84_solid_materials[1] = {
#include "cart/course_art/piece_84_solid_materials.inc"
};
static Sint16 piece_84_solid_indices[14] = {
#include "cart/course_art/piece_84_solid_indices.inc"
};
static NJS_MESHSET piece_84_solid_meshsets[1] = {
#include "cart/course_art/piece_84_solid_meshsets.inc"
};
static NJS_POINT3 piece_84_solid_points[10] = {
#include "cart/course_art/piece_84_solid_points.inc"
};
static NJS_POINT3 piece_84_solid_normals[10] = {
#include "cart/course_art/piece_84_solid_normals.inc"
};
static NJS_MODEL piece_84_solid_model = {
#include "cart/course_art/piece_84_solid_model.inc"
};
static NJS_OBJECT piece_84_solid = {
#include "cart/course_art/piece_84_solid.inc"
};
static NJS_TEXNAME course_texture_names[74] = {
#include "cart/course_art/course_texture_names.inc"
};
NJS_TEXLIST cart_object_textures = {
#include "cart/course_art/cart_object_textures.inc"
};
static struct cart_sign piece_03_signs[2] = {
#include "cart/course_art/piece_03_signs.inc"
};
static struct cart_sign piece_05_signs[2] = {
#include "cart/course_art/piece_05_signs.inc"
};
static struct cart_sign piece_06_signs[2] = {
#include "cart/course_art/piece_06_signs.inc"
};
static struct cart_sign piece_07_signs[13] = {
#include "cart/course_art/piece_07_signs.inc"
};
static struct cart_sign piece_08_signs[13] = {
#include "cart/course_art/piece_08_signs.inc"
};
static struct cart_sign piece_11_signs[10] = {
#include "cart/course_art/piece_11_signs.inc"
};
struct cart_piece cart_pieces[CART_PIECES] = {
#include "cart/course_art/cart_pieces.inc"
};
static struct cart_uv_patch jump_model_patch_1_uv[6] = {
#include "cart/course_art/jump_model_patch_1_uv.inc"
};
static struct cart_texture_patch jump_model_patch_1[1] = {
#include "cart/course_art/jump_model_patch_1.inc"
};
static struct cart_uv_patch jump_model_patch_2_uv[8] = {
#include "cart/course_art/jump_model_patch_2_uv.inc"
};
static struct cart_texture_patch jump_model_patch_2[1] = {
#include "cart/course_art/jump_model_patch_2.inc"
};
static struct cart_uv_patch jump_model_patch_3_uv[34] = {
#include "cart/course_art/jump_model_patch_3_uv.inc"
};
static struct cart_texture_patch jump_model_patch_3[1] = {
#include "cart/course_art/jump_model_patch_3.inc"
};
static struct cart_texture_patch *jump_model_patches[3] = {
#include "cart/course_art/jump_model_patches.inc"
};
static struct cart_uv_patch checkpoint_model_patch_1_uv[10] = {
#include "cart/course_art/checkpoint_model_patch_1_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_1[1] = {
#include "cart/course_art/checkpoint_model_patch_1.inc"
};
static struct cart_uv_patch checkpoint_model_patch_2_uv[40] = {
#include "cart/course_art/checkpoint_model_patch_2_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_2[1] = {
#include "cart/course_art/checkpoint_model_patch_2.inc"
};
static struct cart_uv_patch checkpoint_model_patch_3_uv[32] = {
#include "cart/course_art/checkpoint_model_patch_3_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_3[1] = {
#include "cart/course_art/checkpoint_model_patch_3.inc"
};
static struct cart_uv_patch checkpoint_model_patch_4_uv[24] = {
#include "cart/course_art/checkpoint_model_patch_4_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_4[1] = {
#include "cart/course_art/checkpoint_model_patch_4.inc"
};
static struct cart_uv_patch checkpoint_model_patch_5_uv[40] = {
#include "cart/course_art/checkpoint_model_patch_5_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_5[1] = {
#include "cart/course_art/checkpoint_model_patch_5.inc"
};
static struct cart_uv_patch checkpoint_model_patch_6_uv[16] = {
#include "cart/course_art/checkpoint_model_patch_6_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_6[1] = {
#include "cart/course_art/checkpoint_model_patch_6.inc"
};
static struct cart_uv_patch checkpoint_model_patch_7_uv[40] = {
#include "cart/course_art/checkpoint_model_patch_7_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_7[1] = {
#include "cart/course_art/checkpoint_model_patch_7.inc"
};
static struct cart_uv_patch checkpoint_model_patch_8_uv[26] = {
#include "cart/course_art/checkpoint_model_patch_8_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_8[1] = {
#include "cart/course_art/checkpoint_model_patch_8.inc"
};
static struct cart_uv_patch checkpoint_model_patch_9_uv[8] = {
#include "cart/course_art/checkpoint_model_patch_9_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_9[1] = {
#include "cart/course_art/checkpoint_model_patch_9.inc"
};
static struct cart_uv_patch checkpoint_model_patch_10_uv[4] = {
#include "cart/course_art/checkpoint_model_patch_10_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_10[1] = {
#include "cart/course_art/checkpoint_model_patch_10.inc"
};
static struct cart_uv_patch checkpoint_model_patch_11_uv[4] = {
#include "cart/course_art/checkpoint_model_patch_11_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_11[1] = {
#include "cart/course_art/checkpoint_model_patch_11.inc"
};
static struct cart_uv_patch checkpoint_model_patch_12_uv[4] = {
#include "cart/course_art/checkpoint_model_patch_12_uv.inc"
};
static struct cart_texture_patch checkpoint_model_patch_12[1] = {
#include "cart/course_art/checkpoint_model_patch_12.inc"
};
static struct cart_texture_patch *checkpoint_model_patches[12] = {
#include "cart/course_art/checkpoint_model_patches.inc"
};
static struct cart_uv_patch dash_frame_patch_1_uv[4] = {
#include "cart/course_art/dash_frame_patch_1_uv.inc"
};
static struct cart_texture_patch dash_frame_patch_1[1] = {
#include "cart/course_art/dash_frame_patch_1.inc"
};
static struct cart_uv_patch dash_frame_patch_2_uv[24] = {
#include "cart/course_art/dash_frame_patch_2_uv.inc"
};
static struct cart_texture_patch dash_frame_patch_2[1] = {
#include "cart/course_art/dash_frame_patch_2.inc"
};
static struct cart_texture_patch *dash_frame_patches[2] = {
#include "cart/course_art/dash_frame_patches.inc"
};
static struct cart_uv_patch dash_belt_patch_1_uv[32] = {
#include "cart/course_art/dash_belt_patch_1_uv.inc"
};
static struct cart_texture_patch dash_belt_patch_1[1] = {
#include "cart/course_art/dash_belt_patch_1.inc"
};
static struct cart_texture_patch *dash_belt_patches[1] = {
#include "cart/course_art/dash_belt_patches.inc"
};
static struct cart_uv_patch race_lamp_patch_1_uv[4] = {
#include "cart/course_art/race_lamp_patch_1_uv.inc"
};
static struct cart_texture_patch race_lamp_patch_1[1] = {
#include "cart/course_art/race_lamp_patch_1.inc"
};
static struct cart_texture_patch *race_lamp_patches[1] = {
#include "cart/course_art/race_lamp_patches.inc"
};
static struct cart_uv_patch gate_model_patch_1_uv[8] = {
#include "cart/course_art/gate_model_patch_1_uv.inc"
};
static struct cart_texture_patch gate_model_patch_1[1] = {
#include "cart/course_art/gate_model_patch_1.inc"
};
static struct cart_uv_patch gate_model_patch_2_uv[4] = {
#include "cart/course_art/gate_model_patch_2_uv.inc"
};
static struct cart_texture_patch gate_model_patch_2[1] = {
#include "cart/course_art/gate_model_patch_2.inc"
};
static struct cart_uv_patch gate_model_patch_3_uv[4] = {
#include "cart/course_art/gate_model_patch_3_uv.inc"
};
static struct cart_texture_patch gate_model_patch_3[1] = {
#include "cart/course_art/gate_model_patch_3.inc"
};
static struct cart_uv_patch gate_model_patch_4_uv[8] = {
#include "cart/course_art/gate_model_patch_4_uv.inc"
};
static struct cart_texture_patch gate_model_patch_4[1] = {
#include "cart/course_art/gate_model_patch_4.inc"
};
static struct cart_uv_patch gate_model_patch_5_uv[4] = {
#include "cart/course_art/gate_model_patch_5_uv.inc"
};
static struct cart_texture_patch gate_model_patch_5[1] = {
#include "cart/course_art/gate_model_patch_5.inc"
};
static struct cart_uv_patch gate_model_patch_6_uv[216] = {
#include "cart/course_art/gate_model_patch_6_uv.inc"
};
static struct cart_texture_patch gate_model_patch_6[1] = {
#include "cart/course_art/gate_model_patch_6.inc"
};
static struct cart_uv_patch gate_model_patch_7_uv[30] = {
#include "cart/course_art/gate_model_patch_7_uv.inc"
};
static struct cart_texture_patch gate_model_patch_7[1] = {
#include "cart/course_art/gate_model_patch_7.inc"
};
static struct cart_uv_patch gate_model_patch_8_uv[8] = {
#include "cart/course_art/gate_model_patch_8_uv.inc"
};
static struct cart_texture_patch gate_model_patch_8[1] = {
#include "cart/course_art/gate_model_patch_8.inc"
};
static struct cart_uv_patch gate_model_patch_9_uv[16] = {
#include "cart/course_art/gate_model_patch_9_uv.inc"
};
static struct cart_texture_patch gate_model_patch_9[1] = {
#include "cart/course_art/gate_model_patch_9.inc"
};
static struct cart_texture_patch *gate_model_patches[9] = {
#include "cart/course_art/gate_model_patches.inc"
};
static struct cart_uv_patch final_lamp_patch_1_uv[4] = {
#include "cart/course_art/final_lamp_patch_1_uv.inc"
};
static struct cart_texture_patch final_lamp_patch_1[1] = {
#include "cart/course_art/final_lamp_patch_1.inc"
};
static struct cart_texture_patch *final_lamp_patches[1] = {
#include "cart/course_art/final_lamp_patches.inc"
};
static struct cart_uv_patch sign_model_patch_1_uv[4] = {
#include "cart/course_art/sign_model_patch_1_uv.inc"
};
static struct cart_texture_patch sign_model_patch_1[1] = {
#include "cart/course_art/sign_model_patch_1.inc"
};
static struct cart_uv_patch sign_model_patch_2_uv[4] = {
#include "cart/course_art/sign_model_patch_2_uv.inc"
};
static struct cart_texture_patch sign_model_patch_2[1] = {
#include "cart/course_art/sign_model_patch_2.inc"
};
static struct cart_uv_patch sign_model_patch_3_uv[18] = {
#include "cart/course_art/sign_model_patch_3_uv.inc"
};
static struct cart_texture_patch sign_model_patch_3[1] = {
#include "cart/course_art/sign_model_patch_3.inc"
};
static struct cart_uv_patch sign_model_patch_4_uv[32] = {
#include "cart/course_art/sign_model_patch_4_uv.inc"
};
static struct cart_texture_patch sign_model_patch_4[1] = {
#include "cart/course_art/sign_model_patch_4.inc"
};
static struct cart_uv_patch sign_model_patch_5_uv[12] = {
#include "cart/course_art/sign_model_patch_5_uv.inc"
};
static struct cart_texture_patch sign_model_patch_5[1] = {
#include "cart/course_art/sign_model_patch_5.inc"
};
static struct cart_texture_patch *sign_model_patches[5] = {
#include "cart/course_art/sign_model_patches.inc"
};
static struct cart_uv_patch sign_panel_patch_1_uv[48] = {
#include "cart/course_art/sign_panel_patch_1_uv.inc"
};
static struct cart_texture_patch sign_panel_patch_1[1] = {
#include "cart/course_art/sign_panel_patch_1.inc"
};
static struct cart_texture_patch *sign_panel_patches[1] = {
#include "cart/course_art/sign_panel_patches.inc"
};
struct cart_texture_remap cart_texture_remaps[10] = {
#include "cart/course_art/cart_texture_remaps.inc"
};
