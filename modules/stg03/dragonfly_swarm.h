/* the dragonfly swarm, KAZANI in the stage's object list: dragonflies that
 * fly about a box. each steers toward targets of its own */
#ifndef STG03_DRAGONFLY_SWARM_H
#define STG03_DRAGONFLY_SWARM_H

struct task;

/* the object list's step for KAZANI. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion, which points at room
 * for the dragonflies. it then gives task its own step, draw and destructor.
 * where the heap can't give the record or the room, dragonfly_swarm_init stays
 * the step and tries again.
 *
 * task needs a work. pos is the middle of the swarm's box, ang.y turns the
 * box and scl scales it. the low byte of ang.x, plus 1, is the count of
 * dragonflies. each step caps that byte at 20, but the count stays as the
 * init fixed it. while game_state.time_stopped isn't 0, the dragonflies hold
 * still */
void dragonfly_swarm_init(struct task *task);

#endif
