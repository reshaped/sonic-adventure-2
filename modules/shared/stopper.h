/* the stopper, STOPPER in a stage's object list: a solid block whose lamp
 * blinks. stg10's list calls it MHSLIDER */
#ifndef SHARED_STOPPER_H
#define SHARED_STOPPER_H
#include <ninja.h>

struct task;

/* the stopper's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *stopper_texlists[];

/* the object list's step for STOPPER. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it takes a record from the
 * Shinobi heap for task->extra. where the heap can't give it, stopper_init
 * stays task's step and tries again on its next. the record holds a copy of
 * the stopper's collision node, which goes into the collision world. task
 * then takes the stopper's own step, draw and destructor.
 *
 * task needs a work, whose pos places the stopper and whose ang.y turns it.
 * only a player within the node's radius plus 15 of its center keeps the
 * copy in the world. the lamp is the drawn node's child. it is gray
 * for the first 21 of every 48 steps of game_state.steps, and white for the
 * rest */
void stopper_init(struct task *task);

#endif
