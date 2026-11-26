/* MOVENT and MOVENTF in Sky Rail's object list: a solid model whose child
 * turns */
#ifndef STG06_MOVENT_H
#define STG06_MOVENT_H
#include <ninja.h>

struct task;

/* the model's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *movent_texlists[];

/* the object list's step for MOVENT and MOVENTF. it does nothing where task
 * leaves at its placement's range. otherwise it gives task a copy of the
 * model's collision object from syCalloc in task->motion. the destructor frees
 * the copy. the call then gives task its own step, draw and destructor. where
 * syCalloc fails, the call does nothing, and the next step tries again.
 *
 * task needs a work. its pos places the model, and its angles turn it by
 * njRotateZ, njRotateY and njRotateX, in that order. each later step tests the
 * range as the first did. the model's child turns about z, from a random
 * angle, at a speed the init picks at random. a time stop holds it still.
 *
 * the copy stands in the collision world under WORLD_SOLID and WORLD_NO_CLIMB,
 * turned by ang.y and ang.z alone. a player is near within the collision
 * model's radius plus 15 of pos. each step sets WORLD_ADDED in the work's flags
 * while a player is near, and clears it otherwise */
void movent_init(struct task *task);

#endif
