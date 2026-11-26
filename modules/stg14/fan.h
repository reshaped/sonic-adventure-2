/* the fan, FUN in Radical Highway's object list: a stand whose blades turn
 * about y. the players can stand on the blades */
#ifndef STG14_FAN_H
#define STG14_FAN_H
#include <ninja.h>

struct task;

/* the fan's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_fan_texlists[];

/* the object list's step for FUN. unless placement_try_leave(task) returns
 * TRUE, it gives task the fan's own step, draw and destructor. it also gives
 * task a collision record of the blades' disc.
 *
 * task needs a work. pos places the stand, and ang.y turns it. the disc
 * pushes players, enemies and objects, but carries nothing round with the
 * blades. while game_state.time_stopped is 0, each step turns the blades. that
 * step also plays sound 6 of the stage's list at pos, keyed by the work */
void highway_fan_init(struct task *task);

#endif
