/* the fence, FENCE in Meteor Herd's object list: a rail whose light pulses */
#ifndef STG32_FENCE_H
#define STG32_FENCE_H

#include <ninja.h>

struct task;

extern NJS_TEXLIST herd_fence_texlist;
/* the fence's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *herd_fence_textures[];
/* the object list's step for FENCE. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the fence's step, draw and
 * destructor, and a collision record of kind COLLISION_OBJECT. the record's
 * one shape is the rail's box, which every fence's record shares. where the
 * heap has no room for the record, the fence goes on without one.
 *
 * task needs a work, whose pos places the fence and whose ang.y turns it. each
 * later step tests the range as the first did */
void herd_fence_init(struct task *task);

#endif
