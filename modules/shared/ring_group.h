/* ring groups: up to eight rings placed as one object, along a line or round
 * a circle. an object list names the two kinds RING_LINEAR and RING_CIRCLE,
 * or, in some images, RINGLINE and RINGCIRC */
#ifndef SHARED_RING_GROUP_H
#define SHARED_RING_GROUP_H
#include <ninja.h>

struct task;

/* the rings' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *ring_group_texlists[];

/* the object list's steps for a line and a circle of rings. each does
 * nothing where placement_try_leave(task) returns TRUE. otherwise it gives
 * task the group's own step and destructor, and a record in task->motion.
 * the group's step then makes each ring as a task of its own.
 *
 * task needs a work. a line's first ring stands at pos. a circle's center is
 * pos. ang turns the group about pos. scl.z, truncated and held to 1 through
 * 8, is the count of rings, which the work's tick keeps. scl.x plus 10 is
 * the space between a line's rings, or a circle's radius. a line's rings
 * climb toward a height of scl.y. a circle's rings lie (scl.y + 1) / count
 * turns apart.
 *
 * a ring is spent once a player takes it, or once a scattered ring replaces
 * it. a group never makes a spent ring again. a group with a placement keeps
 * its spent rings in the placement's state, for the placement's later tasks.
 * once every ring is spent, the group ends.
 *
 * where the heap can't give the record, tick keeps its value, and the
 * group's step does nothing */
void ring_linear_init(struct task *task);
void ring_circle_init(struct task *task);

#endif
