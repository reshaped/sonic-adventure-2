/* the canoe, CANOE in Dry Lagoon's object list: a boat that stands in the
 * collision world */
#ifndef STG18_CANOE_H
#define STG18_CANOE_H
#include <ninja.h>

struct task;

/* the canoe's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *canoe_texlists[];

/* the object list's step for CANOE. unless placement_try_leave(task) returns
 * TRUE, it gives task the canoe's own step, draw and destructor. it puts a
 * copy of the canoe's shape in the collision world, under WORLD_SOLID.
 * world_pool_alloc lends the copy, which task->extra keeps. where the pool
 * has none to lend, the call writes through null.
 *
 * task needs a work. its pos places the canoe and the copy, its ang turns
 * both, and scl.x + 1 scales both. the copy collides only while a player lies
 * within 110 * (scl.x + 1) + 10 of pos. the canoe draws under
 * DRAW_CONTROL_SHADOW, so the modifier volumes shadow it */
void canoe_init(struct task *task);

#endif
