/* CHAO.PRS's chunk models and motions, from the disc. its D lies between
 * singing.c and small_toys.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/toy_art.h"

static Sint16 chao_picture_book_node_4_polygons[25] = {
#include "chao/handheld_art/chao_picture_book_node_4_polygons.inc"
};
static Sint32 chao_picture_book_node_4_vertices[27] = {
#include "chao/handheld_art/chao_picture_book_node_4_vertices.inc"
};
static NJS_CNK_MODEL chao_picture_book_node_4_model = {
#include "chao/handheld_art/chao_picture_book_node_4_model.inc"
};
static NJS_CNK_OBJECT chao_picture_book_node_4 = {
#include "chao/handheld_art/chao_picture_book_node_4.inc"
};
static Sint16 chao_picture_book_node_3_polygons[133] = {
#include "chao/handheld_art/chao_picture_book_node_3_polygons.inc"
};
static Sint32 chao_picture_book_node_3_vertices[123] = {
#include "chao/handheld_art/chao_picture_book_node_3_vertices.inc"
};
static NJS_CNK_MODEL chao_picture_book_node_3_model = {
#include "chao/handheld_art/chao_picture_book_node_3_model.inc"
};
static NJS_CNK_OBJECT chao_picture_book_node_3 = {
#include "chao/handheld_art/chao_picture_book_node_3.inc"
};
static Sint16 chao_picture_book_node_2_polygons[131] = {
#include "chao/handheld_art/chao_picture_book_node_2_polygons.inc"
};
static Sint32 chao_picture_book_node_2_vertices[123] = {
#include "chao/handheld_art/chao_picture_book_node_2_vertices.inc"
};
static NJS_CNK_MODEL chao_picture_book_node_2_model = {
#include "chao/handheld_art/chao_picture_book_node_2_model.inc"
};
static NJS_CNK_OBJECT chao_picture_book_node_2 = {
#include "chao/handheld_art/chao_picture_book_node_2.inc"
};
static NJS_CNK_OBJECT chao_picture_book_node_1 = {
#include "chao/handheld_art/chao_picture_book_node_1.inc"
};
NJS_CNK_OBJECT chao_picture_book = {
#include "chao/handheld_art/chao_picture_book.inc"
};
static Sint16 chao_doll_polygons[787] = {
#include "chao/handheld_art/chao_doll_polygons.inc"
};
static Sint32 chao_doll_vertices[483] = {
#include "chao/handheld_art/chao_doll_vertices.inc"
};
static NJS_CNK_MODEL chao_doll_model = {
#include "chao/handheld_art/chao_doll_model.inc"
};
NJS_CNK_OBJECT chao_doll = {
#include "chao/handheld_art/chao_doll.inc"
};
static Sint16 chao_ball_wand_polygons[413] = {
#include "chao/handheld_art/chao_ball_wand_polygons.inc"
};
static Sint32 chao_ball_wand_vertices[297] = {
#include "chao/handheld_art/chao_ball_wand_vertices.inc"
};
static NJS_CNK_MODEL chao_ball_wand_model = {
#include "chao/handheld_art/chao_ball_wand_model.inc"
};
NJS_CNK_OBJECT chao_ball_wand = {
#include "chao/handheld_art/chao_ball_wand.inc"
};
static Sint16 chao_star_wand_polygons[517] = {
#include "chao/handheld_art/chao_star_wand_polygons.inc"
};
static Sint32 chao_star_wand_vertices[603] = {
#include "chao/handheld_art/chao_star_wand_vertices.inc"
};
static NJS_CNK_MODEL chao_star_wand_model = {
#include "chao/handheld_art/chao_star_wand_model.inc"
};
NJS_CNK_OBJECT chao_star_wand = {
#include "chao/handheld_art/chao_star_wand.inc"
};
static Sint16 chao_rattle_polygons[377] = {
#include "chao/handheld_art/chao_rattle_polygons.inc"
};
static Sint32 chao_rattle_vertices[285] = {
#include "chao/handheld_art/chao_rattle_vertices.inc"
};
static NJS_CNK_MODEL chao_rattle_model = {
#include "chao/handheld_art/chao_rattle_model.inc"
};
NJS_CNK_OBJECT chao_rattle = {
#include "chao/handheld_art/chao_rattle.inc"
};
static Sint16 chao_stick_polygons[101] = {
#include "chao/handheld_art/chao_stick_polygons.inc"
};
static Sint32 chao_stick_vertices[99] = {
#include "chao/handheld_art/chao_stick_vertices.inc"
};
static NJS_CNK_MODEL chao_stick_model = {
#include "chao/handheld_art/chao_stick_model.inc"
};
NJS_CNK_OBJECT chao_stick = {
#include "chao/handheld_art/chao_stick.inc"
};
static Sint16 chao_baton_polygons[107] = {
#include "chao/handheld_art/chao_baton_polygons.inc"
};
static Sint32 chao_baton_vertices[75] = {
#include "chao/handheld_art/chao_baton_vertices.inc"
};
static NJS_CNK_MODEL chao_baton_model = {
#include "chao/handheld_art/chao_baton_model.inc"
};
NJS_CNK_OBJECT chao_baton = {
#include "chao/handheld_art/chao_baton.inc"
};
static Sint16 chao_round_toy_polygons[149] = {
#include "chao/handheld_art/chao_round_toy_polygons.inc"
};
static Sint32 chao_round_toy_vertices[219] = {
#include "chao/handheld_art/chao_round_toy_vertices.inc"
};
static NJS_CNK_MODEL chao_round_toy_model = {
#include "chao/handheld_art/chao_round_toy_model.inc"
};
NJS_CNK_OBJECT chao_round_toy = {
#include "chao/handheld_art/chao_round_toy.inc"
};
static Sint16 chao_pogo_stick_polygons[507] = {
#include "chao/handheld_art/chao_pogo_stick_polygons.inc"
};
static Sint32 chao_pogo_stick_vertices[447] = {
#include "chao/handheld_art/chao_pogo_stick_vertices.inc"
};
static NJS_CNK_MODEL chao_pogo_stick_model = {
#include "chao/handheld_art/chao_pogo_stick_model.inc"
};
NJS_CNK_OBJECT chao_pogo_stick = {
#include "chao/handheld_art/chao_pogo_stick.inc"
};
static Sint16 chao_broom_polygons[391] = {
#include "chao/handheld_art/chao_broom_polygons.inc"
};
static Sint32 chao_broom_vertices[309] = {
#include "chao/handheld_art/chao_broom_vertices.inc"
};
static NJS_CNK_MODEL chao_broom_model = {
#include "chao/handheld_art/chao_broom_model.inc"
};
NJS_CNK_OBJECT chao_broom = {
#include "chao/handheld_art/chao_broom.inc"
};
static Sint16 chao_crayon_root_node_1_polygons[125] = {
#include "chao/handheld_art/chao_crayon_root_node_1_polygons.inc"
};
static Sint32 chao_crayon_root_node_1_vertices[93] = {
#include "chao/handheld_art/chao_crayon_root_node_1_vertices.inc"
};
static NJS_CNK_MODEL chao_crayon_root_node_1_model = {
#include "chao/handheld_art/chao_crayon_root_node_1_model.inc"
};
static NJS_CNK_OBJECT chao_crayon_root_node_1 = {
#include "chao/handheld_art/chao_crayon_root_node_1.inc"
};
NJS_CNK_OBJECT chao_crayon_root = {
#include "chao/handheld_art/chao_crayon_root.inc"
};
static Sint16 chao_watering_can_model_polygons[537] = {
#include "chao/handheld_art/chao_watering_can_model_polygons.inc"
};
static Sint32 chao_watering_can_model_vertices[393] = {
#include "chao/handheld_art/chao_watering_can_model_vertices.inc"
};
static NJS_CNK_MODEL chao_watering_can_model_model = {
#include "chao/handheld_art/chao_watering_can_model_model.inc"
};
NJS_CNK_OBJECT chao_watering_can_model = {
#include "chao/handheld_art/chao_watering_can_model.inc"
};
NJS_CNK_OBJECT watering_can_root = {
#include "chao/handheld_art/watering_can_root.inc"
};
static Sint16 chao_crayon_case_polygons[105] = {
#include "chao/handheld_art/chao_crayon_case_polygons.inc"
};
static Sint32 chao_crayon_case_vertices[123] = {
#include "chao/handheld_art/chao_crayon_case_vertices.inc"
};
static NJS_CNK_MODEL chao_crayon_case_model = {
#include "chao/handheld_art/chao_crayon_case_model.inc"
};
NJS_CNK_OBJECT chao_crayon_case = {
#include "chao/handheld_art/chao_crayon_case.inc"
};
static Sint16 chao_crayon_polygons[125] = {
#include "chao/handheld_art/chao_crayon_polygons.inc"
};
static Sint32 chao_crayon_vertices[93] = {
#include "chao/handheld_art/chao_crayon_vertices.inc"
};
static NJS_CNK_MODEL chao_crayon_model = {
#include "chao/handheld_art/chao_crayon_model.inc"
};
NJS_CNK_OBJECT chao_crayon = {
#include "chao/handheld_art/chao_crayon.inc"
};
static Sint16 chao_toy_car_polygons[429] = {
#include "chao/handheld_art/chao_toy_car_polygons.inc"
};
static Sint32 chao_toy_car_vertices[375] = {
#include "chao/handheld_art/chao_toy_car_vertices.inc"
};
static NJS_CNK_MODEL chao_toy_car_model = {
#include "chao/handheld_art/chao_toy_car_model.inc"
};
NJS_CNK_OBJECT chao_toy_car = {
#include "chao/handheld_art/chao_toy_car.inc"
};
static Sint16 chao_shovel_model_polygons[199] = {
#include "chao/handheld_art/chao_shovel_model_polygons.inc"
};
static Sint32 chao_shovel_model_vertices[159] = {
#include "chao/handheld_art/chao_shovel_model_vertices.inc"
};
static NJS_CNK_MODEL chao_shovel_model_model = {
#include "chao/handheld_art/chao_shovel_model_model.inc"
};
NJS_CNK_OBJECT chao_shovel_model = {
#include "chao/handheld_art/chao_shovel_model.inc"
};
NJS_CNK_OBJECT shovel_root = {
#include "chao/handheld_art/shovel_root.inc"
};
static Sint16 bubble_bottle_polygons[105] = {
#include "chao/handheld_art/bubble_bottle_polygons.inc"
};
static Sint32 bubble_bottle_vertices[225] = {
#include "chao/handheld_art/bubble_bottle_vertices.inc"
};
static NJS_CNK_MODEL bubble_bottle_model = {
#include "chao/handheld_art/bubble_bottle_model.inc"
};
NJS_CNK_OBJECT bubble_bottle = {
#include "chao/handheld_art/bubble_bottle.inc"
};
static Sint16 bubble_wand_polygons[87] = {
#include "chao/handheld_art/bubble_wand_polygons.inc"
};
static Sint32 bubble_wand_vertices[75] = {
#include "chao/handheld_art/bubble_wand_vertices.inc"
};
static NJS_CNK_MODEL bubble_wand_model = {
#include "chao/handheld_art/bubble_wand_model.inc"
};
NJS_CNK_OBJECT bubble_wand = {
#include "chao/handheld_art/bubble_wand.inc"
};
static NJS_MKEY_F book_page_motion_joint_1_position[18] = {
#include "chao/handheld_art/book_page_motion_joint_1_position.inc"
};
static NJS_MKEY_A book_page_motion_joint_1_angles[12] = {
#include "chao/handheld_art/book_page_motion_joint_1_angles.inc"
};
static NJS_MKEY_A book_page_motion_joint_2_angles[8] = {
#include "chao/handheld_art/book_page_motion_joint_2_angles.inc"
};
static NJS_MKEY_A book_page_motion_joint_3_angles[6] = {
#include "chao/handheld_art/book_page_motion_joint_3_angles.inc"
};
static NJS_MKEY_A book_page_motion_joint_4_angles[10] = {
#include "chao/handheld_art/book_page_motion_joint_4_angles.inc"
};
static NJS_MDATA2 book_page_motion_tracks[5] = {
#include "chao/handheld_art/book_page_motion_tracks.inc"
};
NJS_MOTION book_page_motion = {
#include "chao/handheld_art/book_page_motion.inc"
};
