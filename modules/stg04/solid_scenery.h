/* White Jungle's solid scenery, JO, JO2, JO_FAR and JO2_FAR in the stage's
 * object list. it is a model of one of three shapes, which stands in the
 * collision world */
#ifndef STG04_SOLID_SCENERY_H
#define STG04_SOLID_SCENERY_H
#include <ninja.h>

struct task;

/* the scenery's texture lists for texture_load_shared, each shape's at its
 * index, ended by null */
extern NJS_TEXLIST *white_jungle_scenery_texlists[];

/* the object list's steps for JO2 and JO2_FAR. JO2's scenery leaves at its
 * placement's range, and JO2_FAR's at 1450. task needs a work. its pos places
 * the scenery, and its ang turns it by njRotateZ, njRotateY and njRotateX. the
 * low byte of scl.x's whole part picks the shape, and a byte past 2 picks 2.
 * the first step then overwrites scl with the model's center in the world.
 * shapes 1 and 2 draw under DRAW_CONTROL_SHADOW, and shape 0 without it.
 *
 * the first step puts a copy of the shape's collision object in the world,
 * under WORLD_SOLID. world_pool_alloc lends the copy, which task->motion keeps.
 * where the pool has none to lend, the first step writes through null. the copy
 * collides only while a player is within the model's radius plus 15 of its
 * center. JO2_FAR's takes 5 in place of 15 */
void white_jungle_scenery_init(struct task *task);
void white_jungle_scenery_far_init(struct task *task);
/* the steps for JO and JO_FAR. each sets scl.x to 0, and then runs JO2's or
 * JO2_FAR's step, so it takes shape 0 */
void white_jungle_scenery_0_init(struct task *task);
void white_jungle_scenery_0_far_init(struct task *task);

#endif
