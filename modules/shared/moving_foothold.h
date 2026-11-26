/* the moving foothold, ASHIBA in a stage's object list. it swings up and down
 * and to and fro, and carries the players on it */
#ifndef SHARED_MOVING_FOOTHOLD_H
#define SHARED_MOVING_FOOTHOLD_H
#include <ninja.h>

struct task;

/* the kinds' texture lists and the shadow's, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *moving_foothold_texlists[];

/* the object list's step for ASHIBA. it first keeps the work's pos in
 * task->extra as the placed pos, which the foothold swings about. it then moves
 * the work's pos along the swings. it does nothing more where
 * placement_try_leave(task) returns TRUE. otherwise task takes the foothold's
 * own step, draw and destructor. a copy of the kind's collision object goes
 * into the collision world. world_pool_alloc lends the copy, and task->motion
 * keeps it.
 *
 * task needs a work, TASK_FORCE's records and TASK_EXTRA's. each step sets both
 * of TASK_FORCE's records to the foothold's move, which carries a player
 * standing on it. the work's fields hold its settings.
 *   ang.x: the speed up and down in the high byte, and to and fro in the low
 *     byte. each counts 4096ths of a turn a step.
 *   ang.y: the direction of the swing to and fro in the high byte, which turns
 *     the foothold too. the swings' phase in the low byte. each counts 256ths
 *     of a turn.
 *   ang.z: the kind in the low byte, mod 6. bit 0 of the high byte turns the
 *     drawn models a further quarter turn about y. the collision object
 *     doesn't take that turn.
 *   scl.x: the scale of the foothold's width and depth, less 1.
 *   scl.y: how far the swing up and down reaches. the foothold rides from the
 *     placed pos's height down to 2 * scl.y below it.
 *   scl.z: how far the swing to and fro reaches, either way from the placed
 *     pos.
 * the foothold's shadow draws at the placed pos's height, under the models'
 * scale and turn */
void moving_foothold_init(struct task *task);

#endif
