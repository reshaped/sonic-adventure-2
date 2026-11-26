/* CHAOSTGRACE.PRS's chunk models, chunk object pointers and personal toys,
 * from the disc. its D lies between map_art.c and camera.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/garden_decor.h"
#include "chao/toy_art.h"
#include "chaostgrace/prize_art.h"

static Sint16 prize_picture_book_polygons[263] = {
#include "chaostgrace/prize_art/prize_picture_book_polygons.inc"
};
static Sint32 prize_picture_book_vertices[267] = {
#include "chaostgrace/prize_art/prize_picture_book_vertices.inc"
};
static NJS_CNK_MODEL prize_picture_book_model = {
#include "chaostgrace/prize_art/prize_picture_book_model.inc"
};
static NJS_CNK_OBJECT prize_picture_book = {
#include "chaostgrace/prize_art/prize_picture_book.inc"
};
static Sint16 prize_crayons_polygons[223] = {
#include "chaostgrace/prize_art/prize_crayons_polygons.inc"
};
static Sint32 prize_crayons_vertices[213] = {
#include "chaostgrace/prize_art/prize_crayons_vertices.inc"
};
static NJS_CNK_MODEL prize_crayons_model = {
#include "chaostgrace/prize_art/prize_crayons_model.inc"
};
static NJS_CNK_OBJECT prize_crayons = {
#include "chaostgrace/prize_art/prize_crayons.inc"
};
static Sint16 prize_dark_ball_polygons[311] = {
#include "chaostgrace/prize_art/prize_dark_ball_polygons.inc"
};
static Sint32 prize_dark_ball_vertices[255] = {
#include "chaostgrace/prize_art/prize_dark_ball_vertices.inc"
};
static NJS_CNK_MODEL prize_dark_ball_model = {
#include "chaostgrace/prize_art/prize_dark_ball_model.inc"
};
static NJS_CNK_OBJECT prize_dark_ball = {
#include "chaostgrace/prize_art/prize_dark_ball.inc"
};
static Sint16 prize_doll_polygons[787] = {
#include "chaostgrace/prize_art/prize_doll_polygons.inc"
};
static Sint32 prize_doll_vertices[483] = {
#include "chaostgrace/prize_art/prize_doll_vertices.inc"
};
static NJS_CNK_MODEL prize_doll_model = {
#include "chaostgrace/prize_art/prize_doll_model.inc"
};
static NJS_CNK_OBJECT prize_doll = {
#include "chaostgrace/prize_art/prize_doll.inc"
};
static Sint16 ball_wand_polygons[413] = {
#include "chaostgrace/prize_art/ball_wand_polygons.inc"
};
static Sint32 ball_wand_vertices[297] = {
#include "chaostgrace/prize_art/ball_wand_vertices.inc"
};
static NJS_CNK_MODEL ball_wand_model = {
#include "chaostgrace/prize_art/ball_wand_model.inc"
};
static NJS_CNK_OBJECT ball_wand = {
#include "chaostgrace/prize_art/ball_wand.inc"
};
static Sint16 star_wand_polygons[503] = {
#include "chaostgrace/prize_art/star_wand_polygons.inc"
};
static Sint32 star_wand_vertices[603] = {
#include "chaostgrace/prize_art/star_wand_vertices.inc"
};
static NJS_CNK_MODEL star_wand_model = {
#include "chaostgrace/prize_art/star_wand_model.inc"
};
static NJS_CNK_OBJECT star_wand = {
#include "chaostgrace/prize_art/star_wand.inc"
};
static Sint16 prize_rattle_polygons[383] = {
#include "chaostgrace/prize_art/prize_rattle_polygons.inc"
};
static Sint32 prize_rattle_vertices[285] = {
#include "chaostgrace/prize_art/prize_rattle_vertices.inc"
};
static NJS_CNK_MODEL prize_rattle_model = {
#include "chaostgrace/prize_art/prize_rattle_model.inc"
};
static NJS_CNK_OBJECT prize_rattle = {
#include "chaostgrace/prize_art/prize_rattle.inc"
};
static Sint16 prize_hero_ball_polygons[373] = {
#include "chaostgrace/prize_art/prize_hero_ball_polygons.inc"
};
static Sint32 prize_hero_ball_vertices[255] = {
#include "chaostgrace/prize_art/prize_hero_ball_vertices.inc"
};
static NJS_CNK_MODEL prize_hero_ball_model = {
#include "chaostgrace/prize_art/prize_hero_ball_model.inc"
};
static NJS_CNK_OBJECT prize_hero_ball = {
#include "chaostgrace/prize_art/prize_hero_ball.inc"
};
static Sint16 prize_pogo_stick_polygons[507] = {
#include "chaostgrace/prize_art/prize_pogo_stick_polygons.inc"
};
static Sint32 prize_pogo_stick_vertices[447] = {
#include "chaostgrace/prize_art/prize_pogo_stick_vertices.inc"
};
static NJS_CNK_MODEL prize_pogo_stick_model = {
#include "chaostgrace/prize_art/prize_pogo_stick_model.inc"
};
static NJS_CNK_OBJECT prize_pogo_stick = {
#include "chaostgrace/prize_art/prize_pogo_stick.inc"
};
static Sint16 prize_rocking_horse_polygons[457] = {
#include "chaostgrace/prize_art/prize_rocking_horse_polygons.inc"
};
static Sint32 prize_rocking_horse_vertices[465] = {
#include "chaostgrace/prize_art/prize_rocking_horse_vertices.inc"
};
static NJS_CNK_MODEL prize_rocking_horse_model = {
#include "chaostgrace/prize_art/prize_rocking_horse_model.inc"
};
static NJS_CNK_OBJECT prize_rocking_horse = {
#include "chaostgrace/prize_art/prize_rocking_horse.inc"
};
static Sint16 prize_broom_polygons[391] = {
#include "chaostgrace/prize_art/prize_broom_polygons.inc"
};
static Sint32 prize_broom_vertices[309] = {
#include "chaostgrace/prize_art/prize_broom_vertices.inc"
};
static NJS_CNK_MODEL prize_broom_model = {
#include "chaostgrace/prize_art/prize_broom_model.inc"
};
static NJS_CNK_OBJECT prize_broom = {
#include "chaostgrace/prize_art/prize_broom.inc"
};
static Sint16 prize_jack_box_polygons[439] = {
#include "chaostgrace/prize_art/prize_jack_box_polygons.inc"
};
static Sint32 prize_jack_box_vertices[609] = {
#include "chaostgrace/prize_art/prize_jack_box_vertices.inc"
};
static NJS_CNK_MODEL prize_jack_box_model = {
#include "chaostgrace/prize_art/prize_jack_box_model.inc"
};
static NJS_CNK_OBJECT prize_jack_box = {
#include "chaostgrace/prize_art/prize_jack_box.inc"
};
static Sint16 prize_watering_can_polygons[533] = {
#include "chaostgrace/prize_art/prize_watering_can_polygons.inc"
};
static Sint32 prize_watering_can_vertices[393] = {
#include "chaostgrace/prize_art/prize_watering_can_vertices.inc"
};
static NJS_CNK_MODEL prize_watering_can_model = {
#include "chaostgrace/prize_art/prize_watering_can_model.inc"
};
static NJS_CNK_OBJECT prize_watering_can = {
#include "chaostgrace/prize_art/prize_watering_can.inc"
};
static Sint16 prize_toy_car_polygons[429] = {
#include "chaostgrace/prize_art/prize_toy_car_polygons.inc"
};
static Sint32 prize_toy_car_vertices[375] = {
#include "chaostgrace/prize_art/prize_toy_car_vertices.inc"
};
static NJS_CNK_MODEL prize_toy_car_model = {
#include "chaostgrace/prize_art/prize_toy_car_model.inc"
};
static NJS_CNK_OBJECT prize_toy_car = {
#include "chaostgrace/prize_art/prize_toy_car.inc"
};
static Sint16 untextured_ball_polygons[127] = {
#include "chaostgrace/prize_art/untextured_ball_polygons.inc"
};
static Sint32 untextured_ball_vertices[255] = {
#include "chaostgrace/prize_art/untextured_ball_vertices.inc"
};
static NJS_CNK_MODEL untextured_ball_model = {
#include "chaostgrace/prize_art/untextured_ball_model.inc"
};
static NJS_CNK_OBJECT untextured_ball = {
#include "chaostgrace/prize_art/untextured_ball.inc"
};
static Sint16 prize_pipe_stand_polygons[467] = {
#include "chaostgrace/prize_art/prize_pipe_stand_polygons.inc"
};
static Sint32 prize_pipe_stand_vertices[483] = {
#include "chaostgrace/prize_art/prize_pipe_stand_vertices.inc"
};
static NJS_CNK_MODEL prize_pipe_stand_model = {
#include "chaostgrace/prize_art/prize_pipe_stand_model.inc"
};
static NJS_CNK_OBJECT prize_pipe_stand = {
#include "chaostgrace/prize_art/prize_pipe_stand.inc"
};
static Sint16 prize_piano_polygons[615] = {
#include "chaostgrace/prize_art/prize_piano_polygons.inc"
};
static Sint32 prize_piano_vertices[621] = {
#include "chaostgrace/prize_art/prize_piano_vertices.inc"
};
static NJS_CNK_MODEL prize_piano_model = {
#include "chaostgrace/prize_art/prize_piano_model.inc"
};
static NJS_CNK_OBJECT prize_piano = {
#include "chaostgrace/prize_art/prize_piano.inc"
};
static Sint16 prize_radio_polygons[531] = {
#include "chaostgrace/prize_art/prize_radio_polygons.inc"
};
static Sint32 prize_radio_vertices[399] = {
#include "chaostgrace/prize_art/prize_radio_vertices.inc"
};
static NJS_CNK_MODEL prize_radio_model = {
#include "chaostgrace/prize_art/prize_radio_model.inc"
};
static NJS_CNK_OBJECT prize_radio = {
#include "chaostgrace/prize_art/prize_radio.inc"
};
static Sint16 prize_shovel_polygons[199] = {
#include "chaostgrace/prize_art/prize_shovel_polygons.inc"
};
static Sint32 prize_shovel_vertices[159] = {
#include "chaostgrace/prize_art/prize_shovel_vertices.inc"
};
static NJS_CNK_MODEL prize_shovel_model = {
#include "chaostgrace/prize_art/prize_shovel_model.inc"
};
static NJS_CNK_OBJECT prize_shovel = {
#include "chaostgrace/prize_art/prize_shovel.inc"
};
static Sint16 prize_bubbles_polygons[201] = {
#include "chaostgrace/prize_art/prize_bubbles_polygons.inc"
};
static Sint32 prize_bubbles_vertices[297] = {
#include "chaostgrace/prize_art/prize_bubbles_vertices.inc"
};
static NJS_CNK_MODEL prize_bubbles_model = {
#include "chaostgrace/prize_art/prize_bubbles_model.inc"
};
static NJS_CNK_OBJECT prize_bubbles = {
#include "chaostgrace/prize_art/prize_bubbles.inc"
};
static Sint16 prize_television_polygons[631] = {
#include "chaostgrace/prize_art/prize_television_polygons.inc"
};
static Sint32 prize_television_vertices[663] = {
#include "chaostgrace/prize_art/prize_television_vertices.inc"
};
static NJS_CNK_MODEL prize_television_model = {
#include "chaostgrace/prize_art/prize_television_model.inc"
};
static NJS_CNK_OBJECT prize_television = {
#include "chaostgrace/prize_art/prize_television.inc"
};
NJS_CNK_OBJECT *race_garden_toys[RACE_PRIZES] = {
#include "chaostgrace/prize_art/race_garden_toys.inc"
};
struct personal_toy race_personal_toys[RACE_PRIZES] = {
#include "chaostgrace/prize_art/race_personal_toys.inc"
};
