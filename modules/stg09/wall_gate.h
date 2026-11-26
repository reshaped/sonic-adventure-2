/* the wall gate, WALL in Prison Lane's object list. its door stands raised
 * while an elevator waits near it */
#ifndef STG09_WALL_GATE_H
#define STG09_WALL_GATE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the gate and of its dust,
 * ended by null */
extern NJS_TEXLIST *wall_gate_texlists[];

/* the object list's step for WALL. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the gate's step, draw
 * and destructor, and a collision record of kind COLLISION_OBJECT. the
 * record's five boxes are the sill, the door, the two posts and the lintel.
 * where the heap has no room for it, the gate goes on without one.
 *
 * task needs a work, whose pos places the gate and whose ang.y turns it. each
 * later step tests the range as the first did. the door stands raised 50 while
 * an elevator of stg09/elevator.h's elevator_tasks waits within 450 of the
 * gate's pos. once none does, the door drops by scl.x a step. it bounces at
 * the sill, and stays shut.
 *
 * the door starts shut where the placement's state isn't 0. the destructor of
 * a shut door sets the state to 1 */
void wall_gate_init(struct task *task);

#endif
