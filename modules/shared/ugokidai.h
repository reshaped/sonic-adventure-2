/* the foothold that bobs up and down in the golem fights, UGOKIDAI in their
 * object lists. it carries the players on it */
#ifndef SHARED_UGOKIDAI_H
#define SHARED_UGOKIDAI_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the kinds' texture lists and the shadow's, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *ugokidai_texlists[];

/* build at buf each model not built yet, if its texture list is loaded. the
 * models are the kinds' and the shadow's. it returns buf past the builds.
 * every foothold draws from the builds once a single call has built all seven.
 * where objects_fog_off isn't 0, the call builds with the fog off, then turns
 * the fog on */
Uint8 *ugokidai_build(Uint8 *buf);

/* the object list's step for UGOKIDAI. it first keeps the work's pos in
 * task->extra as the placed pos, which the foothold bobs below. it then moves
 * the work's pos to where the bob puts it at game_state.steps. it does nothing
 * more where placement_try_leave(task) returns TRUE. otherwise task takes the
 * foothold's own step, draw and destructor. a copy of the kind's collision
 * object goes into the collision world. world_pool_alloc lends the copy, and
 * task->motion keeps it. world_pool_alloc must have an object to lend,
 * unchecked. the non-matching side instead tries again on task's next step,
 * with the foothold back at the placed pos.
 *
 * task needs a work, TASK_FORCE's records and TASK_EXTRA's. each step sets both
 * of TASK_FORCE's records to the foothold's move, which carries a player
 * standing on it. the work's fields hold its settings.
 *   ang.x: the speed of the bob, in 65536ths of a turn a step.
 *   ang.y: the drawn models' turn about y in the high byte. the bob's phase in
 *     the low byte. each counts 256ths of a turn. the collision object turns
 *     by the whole of ang.y.
 *   ang.z: the kind in the low byte, mod 6. kinds 0 to 2 have collision
 *     boxes 30, 50 and 70 high. kinds 3 to 5 are dark models on the same
 *     boxes.
 *   scl.x: the scale of the foothold's width and depth, less 1.
 *   scl.y: how far the bob reaches. the foothold rides from the placed pos's
 *     height down to 2 * scl.y below it.
 * the models and the collision object lie 10 below the work's pos. the
 * foothold's shadow draws at the placed pos's height, under the models' scale
 * and turn */
void ugokidai_init(struct task *task);

#endif
