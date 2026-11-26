/* a garden Chao's walks and turns. the _point behaviors head for
 * mover_set_target's point. the _target ones head for garden_set_target's
 * target */
#ifndef CHAO_WALK_H
#define CHAO_WALK_H

struct task;

void chao_start_skill_walk(struct task *chao);
int chao_stumble_behavior(struct task *chao);
int chao_glide_behavior(struct task *chao);
int chao_choose_destination(struct task *chao);
int chao_pause_with_mark_behavior(struct task *chao);
/* turn toward the garden target for 90 steps */
int chao_face_target_behavior(struct task *chao);
/* turn left or right at even odds, by about 90 to 180 degrees */
int chao_turn_partway_behavior(struct task *chao);
/* place a GARDEN_TOY_ACTIVATE field 3 ahead, which switches on the toy there */
int chao_switch_on_toy_behavior(struct task *chao);
/* turn until within a degree of facing the point */
int chao_face_point_behavior(struct task *chao);
/* walk to within 4 of the point. after 1800 steps, give way to
 * chao_idle_behavior */
int chao_walk_to_point_behavior(struct task *chao);
/* turn left or right at even odds, by about 180 degrees */
int chao_turn_in_place_behavior(struct task *chao);
/* turn left or right at even odds, by up to about 180 degrees */
int chao_turn_around_behavior(struct task *chao);
/* walk to within 4 of the garden target. without a target, or after 1800
 * steps, give way to chao_idle_behavior */
int chao_walk_to_target_behavior(struct task *chao);

#endif
