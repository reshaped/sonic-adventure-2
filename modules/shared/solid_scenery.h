/* the solid scenery, JO and JO_FAR in a stage's object list. it is a model
 * of one of three shapes, which stands in the collision world */
#ifndef SHARED_SOLID_SCENERY_H
#define SHARED_SOLID_SCENERY_H
#include <ninja.h>

struct task;

/* the scenery's texture lists for texture_load_shared, each shape's at its
 * index, ended by null */
extern NJS_TEXLIST *solid_scenery_texlists[];

/* the object list's steps for JO and JO_FAR. JO's scenery leaves at its
 * placement's range, and JO_FAR's at 1450. task needs a work. its pos places
 * the scenery, and its ang turns it by njRotateZ, njRotateY and njRotateX. the
 * low byte of scl.x's whole part picks the shape, and a byte past 2 picks 2.
 * the call then overwrites scl with the model's center in the world.
 *
 * the call puts a copy of the shape's collision object in the world, under
 * WORLD_SOLID. world_pool_alloc lends the copy, which task->motion keeps.
 * where the pool has none to lend, the call writes through null. the copy
 * collides only while a player is within the model's radius plus 5 of its
 * center */
void solid_scenery_init(struct task *task);
void solid_scenery_far_init(struct task *task);

#endif
