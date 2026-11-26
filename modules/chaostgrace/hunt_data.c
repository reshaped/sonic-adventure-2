#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgrace/hunt_data.h"
#include "chaostgrace/props.h"
#include "chaostgrace/scavenger.h"
#include "collision/collision.h"

NJS_CNK_OBJECT *scavenger_panels[SCAVENGER_LANES] = {
#include "chaostgrace/hunt_data/scavenger_panels.inc"
};
NJS_POINT3 scavenger_lane_positions[SCAVENGER_LANES] = {
#include "chaostgrace/hunt_data/scavenger_lane_positions.inc"
};
Sint16 scavenger_questions[TOY_KINDS + 1] = {0, 8, 9, 11, 10};
Sint16 scavenger_answers[3] = {0, 5, 6};
NJS_POINT3 scavenger_item_positions[SCAVENGER_ITEMS] = {
#include "chaostgrace/hunt_data/scavenger_item_positions.inc"
};
const struct collision_shape scavenger_penalty_shape[1] = {
    {(Sint8)RACE_FIELD_PENALTY,
     SHAPE_SPHERE,
     PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3),
     SHAPE_UNTURNED,
     {0.0F, 0.0F, 0.0F},
     5.0F,
     0.0F,
     0.0F,
     0,
     {0, 0, 0}}};
const char scavenger_penalty_name[] = "FallObjectExecutor";
const struct collision_shape scavenger_panel_shape[1] = {
    {(Sint8)198,
     SHAPE_SPHERE,
     PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(3),
     SHAPE_UNTURNED,
     {0.0F, 3.0F, 0.0F},
     5.0F,
     0.0F,
     0.0F,
     0,
     {0, 0, 0}}};
/* the panels' task names. each array rounds its string up to a multiple of
 * 4 bytes */
const char scavenger_question_name[40] =
    "ALO_RaceKarimonoQuestionPanelExecutor";
const char scavenger_answer_name[] = "ALO_RaceKarimonoAnswerPanelExecutor";
const NJS_VECTOR scavenger_drop_velocity = {0.0F, 0.0F, 0.0F};

Bool race_scavenger_submitted[SCAVENGER_LANES];
