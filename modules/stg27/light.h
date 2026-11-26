/* Lost Colony's flashes and light volumes */
#ifndef STG27_LIGHT_H
#define STG27_LIGHT_H
#include <sg_xpt.h>

#include "system/game_state.h"

struct task;

/* the brightness that flashes add this step. the stage's light task adds it
 * to the light's level, then clears it */
extern Float colony_flash;

/* the light the background selects for a player a light volume holds */
#define FLASH_LIGHT 3

/* whether a light volume holds each player this step. the background
 * selects the flash light for such a player, then clears the flag */
extern Bool colony_light_request[GAME_PLAYERS];

/* create a light volume as a child of parent, or return null where no task
 * is free. parent moves the volume's pos. the volume holds each player
 * whose first shape is centered within (scl.x + 1) * 35 of it */
struct task *colony_light_zone_create(struct task *parent);

#endif
