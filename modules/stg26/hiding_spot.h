/* Egg Quarters' hiding volumes, HIDELIGHT and HIDECOLI. the guard counts
 * player 0 hidden while it was inside either at the last step */
#ifndef STG26_HIDING_SPOT_H
#define STG26_HIDING_SPOT_H
#include <sg_xpt.h>

#include "system/game_state.h"

struct task;

/* the volumes' steps, as global_light_switch_init's but for the manager,
 * which the stage creates. HIDECOLI's volume changes no light, and its box,
 * sphere and first cylinder take SHAPE_NO_OTHER_HITS */
void hiding_light_init(struct task *task);
void hiding_collision_init(struct task *task);

/* the managers, which egg_quarters.c creates, one for each kind. each step a
 * manager copies its contacts into previous and clears them */
void hiding_light_manager_init(struct task *task);
void hiding_collision_manager_init(struct task *task);

/* a flag for each player. contacts says whether a volume of the kind touched
 * the player's first shape this step. previous says the same of the step
 * before */
extern Bool hiding_light_contacts[GAME_PLAYERS];
extern Bool hiding_light_previous[GAME_PLAYERS];
extern Bool hiding_collision_contacts[GAME_PLAYERS];
extern Bool hiding_collision_previous[GAME_PLAYERS];

#endif
