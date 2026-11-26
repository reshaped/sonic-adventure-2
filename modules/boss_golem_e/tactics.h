/* the Egg Golem's tactics in BOSS_GOLEM_E.PRS, the dark story's fight. each
 * call does what its twin in boss_golem/tactics.h does, over golem_e and
 * golem_e_floor_y. golem_e_create sets golem_e's dark_story, so player 0 is low
 * below 180 rather than 160. golem_e_set_rest_wait halves the rest wait, and
 * golem_e_may_attack skips the up wait */
#ifndef BOSS_GOLEM_E_TACTICS_H
#define BOSS_GOLEM_E_TACTICS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

struct task *golem_e_wave_create(struct task *parent, int state);
void golem_e_set_rest_wait(void);
Bool golem_e_may_attack(struct task *task);
void golem_e_set_turn(struct task *task, int turn);
void golem_e_aim_slot(struct task *task, int hand);
void golem_e_turn(struct task *task);
void golem_e_set_slot(struct task *task, int slot, int hand);
Bool golem_e_player_facing_up(void);
void golem_e_hover(struct task *task);
Bool golem_e_holding(struct task *task);
void golem_e_count_up_steps(struct task *task);

#endif
