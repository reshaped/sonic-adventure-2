/* the dragonfly, DFLY in the stage's object list. it flies a course of two
 * quarter turns, joined by a straight */
#ifndef STG03_DRAGONFLY_H
#define STG03_DRAGONFLY_H
#include <ninja.h>

struct task;

/* The body's child and that child's sibling are its two wings. */
extern NJS_TEXLIST dragonfly_texlist;
extern NJS_CNK_OBJECT dragonfly_object;

/* the dragonfly's texture list, ended by null. the stage passes the array to
 * texture_load_shared among its groups */
extern NJS_TEXLIST *dragonfly_texlists[];

/* the object list's step for DFLY. unless placement_try_leave(task) returns
 * TRUE, it gives task a record in task->motion. it then gives task its own
 * step, draw and destructor. where the heap can't give the record,
 * dragonfly_init stays the step and tries again.
 *
 * task needs a work. pos is the first turn's center, and ang.y turns the
 * course. scl.x is the turns' radius, scl.y the straight's length and scl.z
 * the speed. the radius must not be 0: a turn's step divides by it.
 *
 * the dragonfly turns a quarter turn about the center and flies the straight.
 * at its end it turns another quarter turn the same way. it then starts again
 * from the course's first point. while game_state.time_stopped isn't 0, it
 * holds its place. its body bobs and rocks with game_state.steps, and its wings
 * beat */
void dragonfly_init(struct task *task);

#endif
