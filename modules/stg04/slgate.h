/* SLGATE in White Jungle's object list: a model the players collide with and
 * the camera passes through */
#ifndef STG04_SLGATE_H
#define STG04_SLGATE_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST slgate_texlist;

/* the object list's step for SLGATE. it leaves at its placement's range. task
 * needs a work, whose pos places the model and whose ang.y turns it.
 *
 * the first step puts a copy of the model's collision object in the world,
 * under WORLD_SOLID and LAND_FAR. camera_collide's probe passes over an object
 * under LAND_FAR. world_pool_alloc lends the copy, which task->motion keeps.
 * where the pool has none to lend, the first step writes through null. the copy
 * collides only while a player lies within 5 of the model's sphere. the
 * destructor takes it out of the world and back to the pool */
void slgate_init(struct task *task);

#endif
