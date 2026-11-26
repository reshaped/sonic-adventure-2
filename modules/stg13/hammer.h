/* the hammer, HAMMER in City Escape's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end */
#ifndef STG13_HAMMER_H
#define STG13_HAMMER_H

#include <ninja.h>

struct task;

/* the hammer's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *hammer_texlists[];
/* the hammer's chunk object, which no code reads. the draw takes
 * hammer_model */
extern NJS_CNK_OBJECT hammer_object;

/* the object list's step for HAMMER. unless placement_try_leave(task)
 * returns TRUE, hammer_init gives task the hammer's own step, draw and
 * destructor. it puts a copy of the hammer's collision object in the
 * collision world. the copy comes from world_pool_alloc, and task->motion
 * keeps it. where the pool is empty, hammer_init writes through null.
 *
 * task needs a work and TASK_FORCE's records. the work's pos is one end of
 * the hammer's stroke. its other fields give the rest:
 * - the low byte of ang.x: the steps it rests at each end.
 * - the high byte of ang.x: the way it strokes. it strokes up from scl.z
 *   below pos where the byte is 0, and down from scl.z above pos otherwise.
 * - the high byte of ang.y: its turn about y.
 * - the low byte of ang.y: its phase, in 256ths of its cycle.
 * - the low half of ang.z: the angle its stroke turns each step. it must not
 *   be 0, unchecked. a stroke is half a turn of a cosine. it lasts 0x8000
 *   divided by that angle, in whole steps.
 * - scl.x and scl.y: its width and its height, each less 1.
 *
 * in a cycle the hammer strokes to pos, rests, strokes back and rests. each
 * step takes its place in the cycle from game_state.steps alone. hammers of
 * the same settings keep in step. the step sets the speed.y of both
 * TASK_FORCE records to the hammer's move, which carries a player riding it.
 * the world collects the copy only while a player lies within the hammer's
 * radius plus 10 of its center. the radius runs from the center to a top
 * corner of a column 50 across and 100 * (scl.y + 1) high. it ignores
 * scl.x */
void hammer_init(struct task *task);

#endif
