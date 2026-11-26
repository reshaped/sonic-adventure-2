/* the scavenger race's panels, signs and fruit, and its data. the data
 * places its lanes and items, and holds its questions, answers, shapes and
 * task names */
#ifndef CHAOSTGRACE_HUNT_DATA_H
#define CHAOSTGRACE_HUNT_DATA_H
#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgrace/scavenger.h"
#include "collision/collision.h"

extern NJS_CNK_OBJECT scavenger_panel_a;
extern NJS_CNK_OBJECT scavenger_panel_b;
extern NJS_CNK_OBJECT scavenger_panel_c;
extern NJS_CNK_OBJECT scavenger_panel_d;
extern NJS_CNK_OBJECT scavenger_sign_a[1], scavenger_sign_b[1];
extern NJS_CNK_OBJECT scavenger_round_fruit;
extern NJS_CNK_OBJECT scavenger_triangle_fruit;
extern NJS_CNK_OBJECT scavenger_square_fruit;
extern NJS_CNK_OBJECT *scavenger_panels[SCAVENGER_LANES];
extern NJS_POINT3 scavenger_lane_positions[SCAVENGER_LANES];
extern NJS_POINT3 scavenger_item_positions[SCAVENGER_ITEMS];
extern Sint16 scavenger_questions[TOY_KINDS + 1], scavenger_answers[3];
extern const struct collision_shape scavenger_penalty_shape[1],
    scavenger_panel_shape[1];
extern const char scavenger_penalty_name[], scavenger_question_name[],
    scavenger_answer_name[];
extern const NJS_VECTOR scavenger_drop_velocity;
#endif
