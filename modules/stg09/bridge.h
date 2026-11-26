/* the bridge, BRIDGE in Prison Lane's object list. its span slides aside out
 * of an elevator's way, and back */
#ifndef STG09_BRIDGE_H
#define STG09_BRIDGE_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the bridge and of its dust,
 * ended by null */
extern NJS_TEXLIST *bridge_texlists[];

/* the object list's step for BRIDGE. it does nothing where task leaves at its
 * placement's range. where the heap can't give its record, it does nothing,
 * and the next step tries again. otherwise it gives task the bridge's step,
 * draw and destructor. it puts a copy of its collision object in the collision
 * world, under WORLD_SOLID and WORLD_MOVING.
 *
 * task needs a work, and TASK_FORCE's records, by which the bridge carries
 * players 0 and 1. the work's pos is the bridge's home, and its ang.y turns
 * it. each later step tests the range as the first did. its aside place lies
 * 140 along its -x. the bridge slides there while an elevator of
 * stg09/elevator.h's elevator_tasks waits within 450 of it. it slides home
 * again while none does. a slide starts only while player 0 lies on the
 * bridge's +x side.
 *
 * the bridge starts aside where bit 0 of the placement's state is set. it never
 * slides where bit 1 is set, and then bobs always. the destructor keeps bit 1,
 * and sets bit 0 where the bridge is home */
void bridge_init(struct task *task);

#endif
