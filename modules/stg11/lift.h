/* the lift, MLIFT in Iron Gate's object list. its top rises once a player
 * has stood on it a while. it sinks back once none has */
#ifndef STG11_LIFT_H
#define STG11_LIFT_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the lift and of its dust,
 * ended by null */
extern NJS_TEXLIST *iron_gate_lift_texlists[];

/* the object list's step for MLIFT. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the lift's step, draw
 * and destructor. it puts a copy of the top's collision object in the
 * collision world, under WORLD_SOLID and WORLD_MOVING. it sets the work's
 * ang.x and ang.z to 0. it gives task a collision record of kind
 * COLLISION_OBJECT, one box under the top. where the heap has no room for the
 * record, the lift goes on without one.
 *
 * task needs a work, and the force records TASK_FORCE allocates. the top
 * carries players 0 and 1 by those records. the work's pos places the lift's
 * base, and its ang.y turns it. each later step tests the range as the first
 * did. the top rises once a player has stood on it for 31 steps in a row. its
 * up end lies scl.x + 20 above the base. it sinks once no player has stood on
 * it for 91 steps.
 *
 * the top starts up where bit 0 of the placement's state is set. where no
 * player stands on it then, it sinks at its first step. where bit 1 is set,
 * the top never moves. its waves bob it while it moves or is held, and stand
 * still while it rests. the destructor keeps bit 1, and sets bit 0 where the
 * top is down */
void iron_gate_lift_init(struct task *task);

#endif
