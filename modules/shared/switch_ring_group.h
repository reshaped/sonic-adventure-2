/* the switch ring groups, SWDRNGL and SWDRNGC in a stage's object list. each
 * is a line or a circle of up to eight rings. the rings appear once a switch
 * id is on */
#ifndef SHARED_SWITCH_RING_GROUP_H
#define SHARED_SWITCH_RING_GROUP_H

#include <ninja.h>

struct task;

/* the texture lists the groups and their rings draw with, ended by null. a
 * stage's texture groups hold the array */
extern NJS_TEXLIST *switch_ring_texlists[];

/* the object list's steps for SWDRNGL and SWDRNGC. task needs a work. a call
 * where placement_try_leave(task) returns TRUE does nothing more. otherwise
 * it gives task a record in task->motion, and the group's own step, draw and
 * destructor. where the heap has no room for the record, task keeps this
 * step, to try again.
 *
 * the work's pos places the group, and the low 16 bits of ang.x and ang.y
 * turn it. the low 4 bits of ang.z are the switch id. the count of rings is
 * scl.z, truncated and held to 1 through 8. the work's tick keeps the count.
 * scl.x plus 10 is the space between a line's rings, or a circle's radius. a
 * line's rings climb toward a height of scl.y. a circle's neighboring rings
 * lie (scl.y + 1) / count turns apart.
 *
 * no ring appears until switch_is_on returns 1 for the id. the nearest
 * player's pos at that step is the group's target. the slot player_nearest
 * gives must hold a player, unchecked. each ring then waits 0.2 steps for
 * each unit of its distance from the target. no ring waits more than 10000
 * steps.
 *
 * a ring is spent once a player takes it, or once a scattered ring replaces
 * it. a group never makes a spent ring again, and ends once every ring is
 * spent. a group with a placement keeps its spent rings in the placement's
 * state. a later task of the placement waits for the switch id again, even
 * where some rings are spent.
 *
 * without a placement, a call that finds the switch id on writes through
 * task->motion before the record exists. the rings' delays then measure from
 * a target that no path sets */
void switch_ring_linear_init(struct task *task);
void switch_ring_circle_init(struct task *task);

#endif
