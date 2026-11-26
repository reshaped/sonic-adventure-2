/* the cell door, CAGEB in Prison Lane's object list. the door sinks into the
 * floor once a group of enemies is beaten */
#ifndef STG09_CAGE_DOOR_H
#define STG09_CAGE_DOOR_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the cell front and of its
 * dust, ended by null */
extern NJS_TEXLIST *cage_door_texlists[];

/* the object list's step for CAGEB. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the door's step, draw
 * and destructor, and a collision record of kind COLLISION_OBJECT. the
 * record's four boxes are the sill, the door and the two posts. where the heap
 * has no room for it, the door goes on without one.
 *
 * task needs a work, whose pos places the cell front and whose ang.y turns it.
 * the low byte of ang.x names a group of enemies, as enemy_group_empty takes
 * it. each later step tests the range as the first did. once the group is
 * empty, the door sinks by scl.x a step. it stops 39 below shut, and bounces
 * there. once the door has sunk, none of the front's boxes collides. while the
 * group isn't empty, the door shuts at once.
 *
 * the door starts sunk where the placement's state isn't 0. the destructor of
 * a sunk door sets the state to 1 */
void cage_door_init(struct task *task);

#endif
