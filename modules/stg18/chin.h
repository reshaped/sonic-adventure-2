/* CHIN in Dry Lagoon's object list: a piece of the ruins that stands in the
 * collision world */
#ifndef STG18_CHIN_H
#define STG18_CHIN_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the piece's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *chin_texlists[];

/* build the piece's model at buf, if it isn't built yet and its texture list
 * is loaded. it returns buf past the build. the piece draws from its build
 * once built, and from its model before. where objects_fog_off isn't 0, the
 * call builds with the fog off, then turns the fog on */
Uint8 *chin_build(Uint8 *buf);

/* the object list's step for CHIN. unless placement_try_leave(task) returns
 * TRUE, it gives task the piece's own step, draw and destructor. it puts a
 * copy of the piece's shape in the collision world, under WORLD_SOLID.
 * world_pool_alloc lends the copy, which task->extra keeps. where the pool
 * has none to lend, the call writes through null.
 *
 * task needs a work. its pos places the piece and the copy, its ang turns
 * both, and scl.x + 1 scales both. the copy collides only while a player lies
 * within 87 * (scl.x + 1) + 10 of pos. the piece draws under
 * DRAW_CONTROL_SHADOW, so the modifier volumes shadow it */
void chin_init(struct task *task);

#endif
