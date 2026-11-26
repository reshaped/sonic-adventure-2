/* the wood fence, SAKU in a stage's object list: a fence that stands in the
 * players' way */
#ifndef SHARED_WOOD_FENCE_H
#define SHARED_WOOD_FENCE_H
#include <ninja.h>

struct task;

/* the fence's texture list, for texture_load_shared, ended by null */
extern NJS_TEXLIST *wood_fence_texlists[];

/* the object list's step for SAKU. unless placement_try_leave(task) returns
 * TRUE, it gives task the fence's step, draw and destructor. it gives the
 * fence a collision record of one box. where the heap has no room for the
 * record, the fence goes on without one. task needs a work.
 *
 * the work's pos places the fence, and its ang.y and ang.z turn it. each axis
 * of the fence and of its box scales by 1 more than that axis of scl. at a
 * scale of 1 the box is 50 long in x, 12 high and 3 deep. its center stands 5
 * above pos */
void wood_fence_init(struct task *task);

#endif
