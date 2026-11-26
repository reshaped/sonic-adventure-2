/* the fan, FUN in Mission Street's object list: a stand whose blades turn
 * about y. the players can stand on the blades */
#ifndef SHARED_STREET_FAN_H
#define SHARED_STREET_FAN_H
#include <ninja.h>

struct task;

/* the fan's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *street_fan_texlists[];

/* the object list's step for FUN. unless placement_try_leave(task) returns
 * TRUE, it gives task the fan's own step, draw and destructor. it also gives
 * task a collision record of the blades' disc.
 *
 * task needs a work. pos places the stand, and ang.y turns it. the disc
 * pushes players, enemies and objects, but carries nothing round with the
 * blades. each step plays sound 14 of the stage's list at pos, keyed by the
 * work */
void street_fan_init(struct task *task);

#endif
