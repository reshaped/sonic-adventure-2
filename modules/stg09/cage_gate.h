/* the cell gate, CAGE in Prison Lane's object list. its door rises once a
 * group of enemies is beaten */
#ifndef STG09_CAGE_GATE_H
#define STG09_CAGE_GATE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the gate and of its dust,
 * ended by null */
extern NJS_TEXLIST *cage_gate_texlists[];

/* the object list's step for CAGE. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the gate's step, draw
 * and destructor, and a collision record of kind COLLISION_OBJECT. the
 * record's one box is the door. where the heap has no room for it, the gate
 * goes on without one.
 *
 * task needs a work, whose pos places the gate and whose ang.y turns it. the
 * low byte of ang.x names a group of enemies, as enemy_group_empty takes it.
 * each later step tests the range as the first did. once the group is empty,
 * the door rises by scl.x a step. it stops 48 above shut, and bounces there.
 * while the group isn't empty, the door shuts at once.
 *
 * the door starts risen where the placement's state isn't 0. the destructor of
 * a risen door sets the state to 1 */
void cage_gate_init(struct task *task);

#endif
