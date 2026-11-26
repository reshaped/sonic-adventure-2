/* the ring groups a flute stand calls up, KDDRNGL and KDDRNGC in a stage's
 * object list. a group is up to eight rings placed as one object, along a
 * line or round a circle */
#ifndef SHARED_KNUDAI_RING_GROUP_H
#define SHARED_KNUDAI_RING_GROUP_H

struct task;

/* the object list's steps for KDDRNGL and KDDRNGC. task needs a work and a
 * placement: a group without one reads placement_get_state's undefined result
 * as it starts. a line's first ring stands at pos, and a circle's center is
 * pos. the low 16 bits of ang.x and ang.y turn the group about pos. ang.z holds
 * the id of the group's stand. scl.z, cut to a whole number and held from 1 to
 * 8, counts the rings. the work's tick keeps the count. scl.x plus 10 is the
 * space between a line's rings, or a circle's radius. a line's rings climb
 * toward a height of scl.y. a circle's rings lie (scl.y + 1) / count turns
 * apart.
 *
 * each step does nothing where placement_try_leave(task) returns TRUE.
 * otherwise it takes a record in task->motion, and gives task the group's step,
 * draw and destructor. where the heap can't give the record, task keeps this
 * step to try again.
 *
 * the rings start to appear once the group's stand is played. each ring then
 * waits 0.8 steps for each unit of its distance from the origin. knudai_get_pos
 * gives the origin for a played stand. a line's rings wait at most 160 steps,
 * and not at all on STAGE_RELAY_SONIC. a circle's rings wait at most 10000
 * steps.
 *
 * a ring is spent once its ring_link, of shared/ring.h, has RING_SPENT. a group
 * never makes a spent ring again. it keeps its spent rings in the placement's
 * state, for the placement's later tasks. once every ring is spent, the group
 * ends. it leaves range only while none of its rings lives */
void knudai_ring_linear_init(struct task *task);
void knudai_ring_circle_init(struct task *task);

#endif
