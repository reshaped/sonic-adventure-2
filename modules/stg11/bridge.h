/* the bridge, BRIDGE in Iron Gate's object list. its span slides aside out of
 * an elevator's way, and back */
#ifndef STG11_BRIDGE_H
#define STG11_BRIDGE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the bridge and of its dust,
 * ended by null */
extern NJS_TEXLIST *iron_gate_bridge_texlists[];

/* the object list's step for BRIDGE. it does nothing where
 * placement_try_leave(task) returns TRUE. where the heap can't give its
 * record, it does nothing, and the next step tries again. otherwise it gives
 * task the bridge's step, draw and destructor. it puts a copy of its collision
 * object in the collision world, under WORLD_SOLID and WORLD_MOVING. each
 * step does nothing where placement_try_leave(task) returns TRUE.
 *
 * task needs a work, and TASK_FORCE's records, by which the bridge carries
 * players 0 and 1. the work's pos is the bridge's home, and its ang.y turns
 * it. its aside place lies 140 along its -x. the bridge slides there while an
 * elevator of iron_gate_elevator_tasks waits within 450 of it. it slides home
 * again while none does. it starts a slide only 120 steps from the last step
 * it wanted to slide. it slides 8 a step, and bobs as it goes.
 *
 * the bridge starts aside where bit 0 of the placement's state is set. it never
 * slides where bit 1 is set, and then bobs always. the destructor keeps bit 1,
 * and sets bit 0 where the bridge is home */
void iron_gate_bridge_init(struct task *task);

#endif
