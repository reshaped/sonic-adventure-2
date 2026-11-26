/* the hatches in Eternal Engine's object list: HATCH_A in a floor, and HATCH_B
 * in a wall. a hatch opens once its dynamite is gone, and then pulls the
 * players into it */
#ifndef STG24_HATCH_H
#define STG24_HATCH_H
#include <sg_xpt.h>

struct task;

/* build the hatches' nine models at buffer, and return the end of the builds.
 * a hatch made after a call that built all nine draws the builds. any other
 * draws the models as chunks */
Uint8 *hatches_build(Uint8 *buffer);
/* the object list's steps for HATCH_A and HATCH_B. each does nothing where
 * task leaves at its placement's range. where the heap can't give its record,
 * it does nothing, and the next step tries again. otherwise it gives task the
 * hatch's step, draw and destructor. it puts the hatch's collision model in
 * the world, and makes task a field.
 *
 * task needs a work. pos places the hatch, and ang.y turns it. the low byte of
 * ang.x is the id of the dynamite charges that hold it shut. a step that finds
 * none of them left opens the hatch, and one that finds any shuts it. id 0
 * keeps it shut. an open hatch's lid sinks. once it clears the mouth, the
 * field pulls the players near into the hatch. the field kills deep in a
 * hatch. the wall's also kills low in or before its mouth. stg24_step makes a
 * player at such a point fall out of the stage.
 *
 * an open hatch makes its air as task's child. a shut wall hatch kills the
 * child, and a shut floor hatch keeps it */
void floor_hatch_init(struct task *task);
void wall_hatch_init(struct task *task);

#endif
