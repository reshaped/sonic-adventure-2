/* PILLAR4 in a stage's object list: a pillar whose object stands in the
 * collision world */
#ifndef SHARED_PILLAR4_H
#define SHARED_PILLAR4_H
#include <ninja.h>

struct task;

/* the pillar's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pillar4_texlists[];

/* the object list's step for PILLAR4. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * pillar's own step, draw and destructor. it then puts a copy of
 * pillar4_object in the world under WORLD_SOLID, in task->extra, from
 * world_pool_alloc. the pool must have an object to lend, unchecked. the
 * destructor takes the copy out of the world and back to the pool.
 *
 * task needs a work, whose pos places the pillar and whose ang.y turns it.
 * the pillar's scale is 1 more than scl.x. the world takes the copy only
 * while a player lies within 153 times the scale, plus 15. the pillar draws
 * under DRAW_CONTROL_SHADOW */
void pillar4_init(struct task *task);
extern NJS_CNK_MODEL pillar4_model;
extern NJS_OBJECT pillar4_object;
extern NJS_TEXLIST pillar4_texlist;

#endif
