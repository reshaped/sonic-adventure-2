/* the gravity field of Meteor Herd: a sphere or a capsule. it points
 * world_gravity from a player within its reach toward itself. world_gravity is
 * the one way things fall, which every player shares. no object list names
 * it */
#ifndef STG32_GRAVITY_H
#define STG32_GRAVITY_H

struct task;

/* the field's init. it does nothing where task leaves at its placement's
 * range. otherwise it gives task the field's step, draw and destructor. task
 * needs a work, and each later step tests the range as the first did.
 *
 * the field is a sphere about pos. with bit 0 of ang.x set, it is a capsule
 * about a segment through pos. the segment runs scl.y each way along y. ang.z
 * turns it about z, and then ang.y about y.
 *
 * each step, a player within scl.x + 10 of the center or the segment sets
 * world_gravity. it becomes the unit vector from the player toward the center,
 * or the segment's nearest point. the step sets world_gravity_ang_x and
 * world_gravity_ang_z to match. where both players are in range, player 1's
 * gravity stands */
void herd_gravity_init(struct task *task);

#endif
