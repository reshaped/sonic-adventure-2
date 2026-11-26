/* the store box, STORE in Death Chamber and Egg Quarters: a box that the
 * players stand on. it is blue, green or red */
#ifndef SHARED_STORE_BOX_H
#define SHARED_STORE_BOX_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the box's texture lists for texture_load_shared, a color's at its index,
 * ended by null */
extern NJS_TEXLIST *store_box_texlists[];

/* build at buf each color's box that isn't built yet, if its texture list is
 * loaded. it returns buf past the builds. a box draws from its build once a
 * call has built all three, and from its model before. where objects_fog_off
 * isn't 0, the call builds with the fog off, then turns the fog on */
Uint8 *store_box_build(Uint8 *buf);

/* the object list's step for STORE. unless placement_try_leave(task)
 * returns TRUE, it gives task the box's own step, draw and destructor. it puts
 * a copy of the box's shape in the collision world, under WORLD_SOLID.
 * world_pool_alloc lends the copy, which task->extra keeps. where the pool has
 * none to lend, the call writes through null.
 *
 * task needs a work. its pos places the box, and its ang.y turns it. the copy
 * in the world takes all three of the work's angles, and scl.x + 1 scales
 * both. the color is the whole part of |scl.z|, modulo 3: 0 is blue, 1 green
 * and 2 red. the copy stays in the world only while a player lies within
 * 67.8 of the box's pos. the box draws under DRAW_CONTROL_SHADOW, so the
 * modifier volumes shadow it */
void store_box_init(struct task *task);

#endif
