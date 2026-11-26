/* PALM in Dry Lagoon's object list: a palm tree that stands in the collision
 * world */
#ifndef STG18_PALM_H
#define STG18_PALM_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the palm's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *palm_texlists[];

/* build the palm's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the build. the palm draws from its build
 * once built, and from its model before. where objects_fog_off isn't 0, the
 * call builds with the fog off, then turns the fog on */
Uint8 *palm_build(Uint8 *buf);

/* the object list's step for PALM. unless placement_try_leave(task) returns
 * TRUE, it gives task the palm's own step, draw and destructor. it puts a
 * copy of the palm's shape in the collision world, under WORLD_SOLID.
 * world_pool_alloc lends the copy, which task->extra keeps. where the pool
 * has none to lend, the call writes through null. it also makes 150 the
 * radius of the model all palms share.
 *
 * task needs a work. its pos places the palm and the copy, and scl.x + 1
 * scales both. its ang turns both about z, then x, then y. the copy collides
 * only while a player lies within 87 * (scl.x + 1) + 10 of pos. the palm
 * draws under DRAW_CONTROL_SHADOW, so the modifier volumes shadow it */
void palm_init(struct task *task);

#endif
