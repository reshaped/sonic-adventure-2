/* Crazy Gadget's GLIND, a rail to grind */
#ifndef STG22_GRIND_RAIL_H
#define STG22_GRIND_RAIL_H
#include <ninja.h>

struct task;

/* the object list's init for GLIND. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the rail's step, draw and
 * destructor. it copies the rail's path, placed for the rail and for gravity.
 * a child of task that path_rail_init starts takes the copy. a rail whose
 * scl.x is below 0.0001 carries no rider. nor does one whose copy or child
 * the heap can't give.
 *
 * task needs a work. pos places the rail, and ang.y turns it. the rail runs
 * 1 + scl.x times its model's length along its x. ang.x turns its two end
 * pieces about their x. the draw's cull takes the model's radius times 1
 * plus the larger of scl.x and scl.y. a step that finds gravity's angles
 * changed since the init kills the rail */
void gadget_grind_rail_init(struct task *task);
extern NJS_TEXLIST *gadget_grind_rail_texlists[];

#endif
