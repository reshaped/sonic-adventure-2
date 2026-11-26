/* the fence, FENCE in Mad Space's object list: a rail whose light scrolls */
#ifndef STG44_FENCE_H
#define STG44_FENCE_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST mad_space_fence_texlist;
/* the fence's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *mad_space_fence_textures[];
/* the object list's step for FENCE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the fence's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record's
 * one shape is the rail's box. where the heap has no room for the record, the
 * fence goes on without one.
 *
 * task needs a work, whose pos places the fence and whose ang.y turns it. the
 * light scrolls along u by scl.x, cut to a whole number, each step. each
 * later step tests the range as the first did */
void mad_space_fence_init(struct task *task);

#endif
