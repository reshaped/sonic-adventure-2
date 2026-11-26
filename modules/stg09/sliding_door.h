/* the sliding door, CAGED in Prison Lane's object list. the cell door slides
 * aside once a group of enemies is beaten */
#ifndef STG09_SLIDING_DOOR_H
#define STG09_SLIDING_DOOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the door and of its dust,
 * ended by null */
extern NJS_TEXLIST *sliding_door_texlists[];

/* the object list's step for CAGED. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the door's step, draw
 * and destructor, and a collision record of kind COLLISION_OBJECT. the
 * record's one box is the door. where the heap has no room for it, the door
 * goes on without one.
 *
 * task needs a work, whose pos places the door and whose ang.y turns it. the
 * low byte of ang.x names a group of enemies, as enemy_group_empty takes it.
 * each later step tests the range as the first did. once the group is empty,
 * the door slides by scl.x a step along its z. it slides toward +z where bit 0
 * of ang.z is set, and toward -z where it isn't. it stops 50 aside, and
 * bounces there. while the group isn't empty, the door shuts at once.
 *
 * the door starts open where the placement's state isn't 0. it then starts 60
 * aside, and its first step brings it back to 50. the destructor of an open
 * door sets the state to 1 */
void sliding_door_init(struct task *task);

#endif
