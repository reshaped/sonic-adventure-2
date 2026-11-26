/* the foothold, ASHIBA in Mad Space's object list: a floor no player can
 * climb, whose lamp pulses */
#ifndef STG44_FOOTHOLD_H
#define STG44_FOOTHOLD_H

#include <ninja.h>

struct task;

/* the object list's step for ASHIBA. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the foothold's step, draw and
 * destructor. it puts a copy of mad_space_foothold_collision into the world
 * as WORLD_SOLID and WORLD_NO_CLIMB. the work's pos and three angles place the
 * copy. where the heap can't give the copy, it tries again at task's next
 * step.
 *
 * task needs a work. a scl.x of 1 or more draws the alternate model, and less
 * the plain one. both collide alike. the work keeps WORLD_ADDED while a player
 * lies within 80 of pos */
void mad_space_foothold_init(struct task *task);
/* the two models' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_foothold_textures[];

#endif
