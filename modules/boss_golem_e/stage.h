/* the Egg Golem stage of the dark story, the program of BOSS_GOLEM_E.PRS. the
 * overlay's head names its init, its exit and its task's step */
#ifndef BOSS_GOLEM_E_STAGE_H
#define BOSS_GOLEM_E_STAGE_H

#include "object/set.h"

struct task;

/* start the stage, and compile the land's and the objects' models. it gives
 * player_hooks.drop_rings ring_scatter and player_hooks.draw_blast_ring
 * bomb_hat_draw_ring where each is null, and sets stage_hooks. of its calls'
 * results it tests only the background task's */
void golem_e_stage_init(void);
/* end the stage. it releases the textures, the messages and the set file. it
 * stops the particles, the set manager, the land and the stage's motions. it
 * clears player_hooks.draw_blast_ring where that still holds
 * bomb_hat_draw_ring, and player_hooks.drop_rings where that still holds
 * ring_scatter */
void golem_e_stage_exit(void);
/* the stage task's step. the first step creates the golem, the reels and the
 * footholds. the task's later steps hold player 0 inside the arena. on the
 * non-matching side, a step that can't create the golem or the footholds tries
 * them again */
void golem_e_stage_step(struct task *task);
/* the stage's object list, which golem_e_stage_init gives to set_start */
extern struct object_list golem_e_object_list;

#endif
