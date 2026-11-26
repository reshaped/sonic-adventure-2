/* the floating foothold, UKIASIBA in Aquatic Mine's object list. it floats on
 * the water of stg07/water.h. a player who lands on it sinks and tilts it, and
 * it springs back */
#ifndef STG07_FLOATING_FOOTHOLD_H
#define STG07_FLOATING_FOOTHOLD_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the foothold and of its
 * ripples, ended by null */
extern NJS_TEXLIST *floating_foothold_texlists[];

/* the object list's step for UKIASIBA. it does nothing where task leaves at
 * its placement's range. where the heap can't give its record, it does
 * nothing, and the next step tries again. otherwise it gives task the
 * foothold's step, draw and destructor. it puts a copy of its collision object
 * in the collision world. each later step tests the range as the first did.
 *
 * task needs a work and TASK_FORCE's records, by which the foothold moves the
 * players that ride it. pos places the foothold, and ang.y turns it. the
 * foothold stays within scl.y of pos's height, above or below */
void floating_foothold_init(struct task *task);

#endif
