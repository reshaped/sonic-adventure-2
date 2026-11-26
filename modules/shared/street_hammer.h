/* the hammer, HAMMER in Mission Street's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end. it can cast a shadow to the floor below it */
#ifndef SHARED_STREET_HAMMER_H
#define SHARED_STREET_HAMMER_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/road_furniture/hammer.h"

struct task;

/* the hammer's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *street_hammer_texlists[];

/* the object list's step for HAMMER. unless placement_try_leave(task)
 * returns TRUE, street_hammer_init gives task the hammer's own step, draw and
 * destructor. it puts a copy of street_hammer_collision in the collision
 * world. world_pool_alloc lends the copy, which task->motion keeps. where the
 * pool has none to lend, street_hammer_init writes through null.
 *
 * task needs a work, TASK_FORCE's records and TASK_EXTRA's record. the work's
 * pos is one end of the hammer's stroke. its other fields give the rest:
 * - the low byte of ang.x: the steps it rests at each end.
 * - the high byte of ang.x: the way it strokes. it strokes up from scl.z
 *   below pos where the byte is 0, and down from scl.z above pos otherwise.
 * - the high byte of ang.y: its turn about y.
 * - the low byte of ang.y: its phase, in 256ths of its cycle.
 * - the low 12 bits of ang.z: the angle its stroke turns each step. it must
 *   not be 0, unchecked. a stroke is half a turn of a cosine. it lasts
 *   0x8000 divided by that angle, in whole steps.
 * - the high 4 bits of ang.z: a shadow, where any is set.
 * - scl.x and scl.y: its width and its height, each less 1.
 *
 * in a cycle the hammer strokes to pos, rests, strokes back and rests. each
 * step takes its place in the cycle from game_state.steps alone. hammers of the
 * same settings keep in step. the step sets the speed.y of both
 * TASK_FORCE records to the hammer's move, which carries a player riding it.
 * only a player within the hammer's radius plus 10 of its center keeps the
 * copy in the world. the radius ignores scl.x. it reaches a top corner of a
 * column 50 across and 100 * (scl.y + 1) high.
 *
 * the glow pulses every 64 steps. a shadow is a modifier volume down toward
 * the floor below the hammer. the height it reaches toward is whatever R0
 * holds as the floor's probe returns, not the probe's result */
void street_hammer_init(struct task *task);

/* return the WORLD_SIZE_CLASS flags of the class that radius falls in.
 * collision/world.h gives each class's bounds */
Uint32 street_hammer_size_class(Float radius);

#endif
