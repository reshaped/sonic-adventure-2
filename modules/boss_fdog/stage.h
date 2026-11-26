/* the Flying Dog stage, the program of BOSS_FDOG.PRS. the overlay's head
 * names its init, its exit and its task's step */
#ifndef BOSS_FDOG_STAGE_H
#define BOSS_FDOG_STAGE_H
#include "object/set.h"

struct task;

/* start the stage and its fog, and compile the land's models. it gives
 * player_hooks.drop_rings ring_scatter where that is null, and sets
 * stage_hooks. it tests no call's result */
void fdog_stage_init(void);
/* end the stage. it releases the textures, the messages and the set files.
 * it stops the fog, the particles, the set manager and the land. it turns
 * Ninja's fog off and sets the back color to 0. it clears
 * player_hooks.drop_rings where that still holds ring_scatter */
void fdog_stage_exit(void);
/* the stage task's step. the first step creates the Flying Dog, and the
 * task's later steps hold player 0 inside the arena */
void fdog_stage_step(struct task *task);
/* the stage's object list, which fdog_stage_init gives to set_start */
extern struct object_list fdog_object_list;

#endif
