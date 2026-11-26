/* the pole, POLE in Wild Canyon's object list: a pillar that the players
 * collide with */
#ifndef SHARED_POLE_H
#define SHARED_POLE_H
#include <ninja.h>

struct task;

/* the pole's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *pole_texlists[];

/* the object list's step for POLE. it does nothing where half_try_leave or
 * placement_try_leave returns TRUE for task. otherwise it gives task the pole's
 * own step, draw and destructor. it then puts a copy of the pole's shape in the
 * world under WORLD_SOLID, in task->extra, from world_pool_alloc. the pool must
 * have an object to lend, unchecked. the destructor takes the copy out of the
 * world and back to the pool.
 *
 * task needs a work, whose pos places the pole and whose ang.y turns it. the
 * pole's scale in x and z is 1 more than scl.x. its scale in y is 1 more than
 * scl.y. the world takes the copy only while a player lies within 100.6382
 * times the two scales, plus 10 */
void pole_init(struct task *task);

#endif
