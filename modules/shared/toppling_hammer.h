/* the toppling hammer, HAMMER3 in the object lists of both Mission Street
 * stages. it stands until its charges blow. it then topples over an edge of
 * its base, rocks and comes to rest */
#ifndef SHARED_TOPPLING_HAMMER_H
#define SHARED_TOPPLING_HAMMER_H

struct task;

/* the object list's step for HAMMER3. unless placement_try_leave(task)
 * returns TRUE, it gives task a child, which carries the hammer's collision
 * record. where task_create_child fails, it ends task with task_kill.
 * otherwise it gives task the hammer's own step, draw and destructor. it puts
 * a copy of street_hammer_collision in the collision world, as
 * street_hammer_init does. world_pool_alloc lends the copy, and the heap gives
 * a matrix, both of which TASK_EXTRA's record keeps. the pool must have a
 * copy to lend, and the heap room for the matrix, each unchecked.
 *
 * task needs a work, a placement, TASK_MOTION's record, TASK_FORCE's records
 * and TASK_EXTRA's record. the work gives the hammer's settings:
 * - bits 4 to 13 of ang.x: its lean. the hammer comes to rest that angle
 *   short of a quarter turn. a hammer of lean 0 falls flat, and hurts the
 *   players in its way as it falls. it also casts a shadow, and raises dust
 *   as it lands.
 * - ang.y: its turn about y.
 * - the low byte of ang.z: the id of the dynamite charges it waits for. it
 *   falls once dynamite_count returns 0 for the id. at 0 it falls once
 *   enemy_hit_hook, where set, returns nonzero for its pos.
 * - scl.x and scl.y: its thickness and its height, each less 1.
 *
 * a hammer whose placement's state isn't 0 starts lying at its stop. the
 * landing sets the state to 1 and plays a sound. it shakes camera 0 and
 * vibrates player 0's pad by player 0's distance from the hammer */
void toppling_hammer_init(struct task *task);

#endif
