/* warp holes, tasks named WpHoleExec: swarms of points that swirl into a core
 * or out of it. the points draw as glowing sprites. warp_hole_particle_create
 * makes particles of the same texture */
#ifndef SHARED_WARP_HOLE_H
#define SHARED_WARP_HOLE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"

struct task;

/* one texture, named sikake_37_32, for the holes' sprites and the particles */
extern NJS_TEXLIST warp_hole_texlist;

/* make a hole of count points at pos, in a task named WpHoleExec, and return
 * the task. it returns null where task_create fails. where syCalloc fails for
 * the hole's record or arrays, it destroys the task and returns null. it
 * returns null too where placement_try_leave returns TRUE for the new task.
 * that task then destroys itself on its next step. each step of the hole calls
 * placement_try_leave too, and the hole ends where it returns TRUE.
 *
 * count must be at least 1, as njPtclDrawSprite requires. the points swirl in
 * to a core, or out from it where outward isn't 0. the core's radius is 0.07
 * times scale, and the hole's edge lies scale beyond the core. scale must not
 * be 0, or an outward hole's points become NaN.
 *
 * size is the w and h of the largest sprites, as njPtclDrawSprite takes them.
 * the task keeps scale and size where WARP_HOLE_SCALE and WARP_HOLE_SIZE read
 * them, and a caller may change either. a point the hole places anew takes the
 * scale then, and each draw takes the size.
 *
 * where owner isn't null, the hole stores its task in *owner. its destructor
 * clears *owner while *owner still holds the task */
struct task *warp_hole_create(NJS_POINT3 *pos, Float scale, int count,
                              Uint8 outward, Float size, struct task **owner);
/* warp_hole_create's scale and size, in the task it returns */
/* @hack type: force read as a Float through its address, where (task)->force.f
 * would do */
#define WARP_HOLE_SCALE(task) (*(Float *)&(task)->force)
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define WARP_HOLE_SIZE(task) (*(Float *)&(task)->extra)
/* make a particle at pos, moving at spd, at a size of scale. return it, or null
 * where the heap has no room for one. the particle spins and drifts, slows and
 * rises, and shrinks and fades. it ends once its alpha falls below 10 or its
 * scale below 0 */
struct particle *warp_hole_particle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale);

#endif
