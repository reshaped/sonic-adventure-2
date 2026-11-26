/* the guards: two robots that patrol Egg Quarters. their radar tints the fog
 * as one nears player 0 */
#ifndef STG26_GUARD_H
#define STG26_GUARD_H
#include <sg_xpt.h>

#include "shared/fog.h"

struct task;

/* the radar's fog, which the stage reads from stg26_fogB.bin. the radar sets
 * its color and speed. while guard_radar_on is TRUE, the stage shows it in
 * place of stg26_fogC.bin's. it does so only while the camera lies above the
 * water */
extern struct fog_setting guard_radar_fog;
extern Bool guard_radar_on;

/* compile the guard's model at buffer, and return the byte after it. where
 * the compile fails, the guards draw the model uncompiled. the call sets
 * DRAW_CONTROL_SHADOW for the build, and clears it after. while
 * objects_fog_off isn't 0, it turns the fog off for the build and on after
 * it */
Uint8 *guard_build(Uint8 *buffer);

/* start the two guards, half the path apart. guard_radar_create must come
 * first. the heap must have room for each guard's state and matrix,
 * unchecked */
void guards_start(void);

/* start the radar's task, with guard_radar_on FALSE. the call forgets every
 * guard. it returns the task, or null where none can be created */
struct task *guard_radar_create(void);

/* whether player 0 touched a HIDELIGHT in the last round of collisions */
Bool guard_player_in_shadow(void);

#endif
