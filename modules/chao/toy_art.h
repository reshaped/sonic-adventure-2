/* the toys' models, motions and texture list, which garden and race objects
 * share */
#ifndef CHAO_TOY_ART_H
#define CHAO_TOY_ART_H
#include <ninja.h>

#include "draw/motion.h"

extern NJS_TEXLIST garden_toy_texlist;
extern NJS_CNK_MODEL garden_shadow_model;
extern NJS_CNK_OBJECT garden_shadow_object;
extern NJS_CNK_MODEL garden_jack_model;
/* @hack type: the box declared as an array of one, where the box alone would
 * do */
extern NJS_CNK_OBJECT garden_jack_box[1];
/* @hack type: the lid's motion declared as an array of one, where the motion
 * alone would do */
extern NJS_MOTION garden_jack_lid_motion[1];
extern NJS_CNK_OBJECT garden_horse_model;
extern struct motion_table_entry garden_horse_clips[5];

extern NJS_CNK_OBJECT garden_neutral_ball_model;
extern NJS_CNK_OBJECT garden_hero_ball_model;
extern NJS_CNK_OBJECT garden_dark_ball_model;
extern NJS_CNK_OBJECT garden_radio_model;
extern NJS_CNK_OBJECT garden_radio_root;
/* @hack type: the screen declared as an array of one, where the screen alone
 * would do */
extern NJS_CNK_OBJECT garden_tv_screen[1];
extern NJS_CNK_OBJECT garden_tv_model;
extern NJS_CNK_OBJECT garden_tv_root;
extern NJS_CNK_OBJECT chao_picture_book;
extern NJS_CNK_OBJECT chao_doll;
extern NJS_CNK_OBJECT chao_ball_wand;
extern NJS_CNK_OBJECT chao_star_wand;
extern NJS_CNK_OBJECT chao_rattle;
extern NJS_CNK_OBJECT chao_stick;
extern NJS_CNK_OBJECT chao_baton;
extern NJS_CNK_OBJECT chao_round_toy;
extern NJS_CNK_OBJECT chao_pogo_stick;
extern NJS_CNK_OBJECT chao_broom;
extern NJS_CNK_OBJECT chao_crayon_root;
extern NJS_CNK_OBJECT chao_watering_can_model;
extern NJS_CNK_OBJECT watering_can_root;
extern NJS_CNK_OBJECT chao_crayon_case;
extern NJS_CNK_OBJECT chao_crayon;
extern NJS_CNK_OBJECT chao_toy_car;
extern NJS_CNK_OBJECT chao_shovel_model;
extern NJS_CNK_OBJECT shovel_root;
extern NJS_CNK_OBJECT bubble_bottle;
extern NJS_CNK_OBJECT bubble_wand;
extern NJS_MOTION book_page_motion;

#endif
