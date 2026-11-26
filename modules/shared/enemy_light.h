/* the lights the enemies show: glows that fade from full and end */
#ifndef SHARED_ENEMY_LIGHT_H
#define SHARED_ENEMY_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* load the lights' texture, st_c20, unless a task holds it already. that
 * task runs at task level 0, and releases the texture when it is destroyed.
 * the heap must have room for the task, unchecked */
void enemy_light_load_texture(void);
/* show a light of kind at pos, which fades from full and ends. kind's low
 * byte sets the light's size and how fast it fades. a byte outside 1 to 4
 * acts as 0. the light is parent's child where parent isn't null, and else a
 * task at task level 3. the call loads the texture as
 * enemy_light_load_texture does. where the heap has no room for the light's
 * task, the call does nothing.
 *
 * a light also ends with parent, or, without a parent, once
 * placement_try_leave finds it out of range. while game_state.time_stopped is
 * set, a light neither fades nor ends for having faded */
void enemy_light_create(struct task *parent, int kind, NJS_POINT3 *pos);

#endif
