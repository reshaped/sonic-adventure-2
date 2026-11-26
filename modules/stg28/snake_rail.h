/* the snake rail, SNEAKRAIL in Pyramid Cave's object list: a solid rail */
#ifndef STG28_SNAKE_RAIL_H
#define STG28_SNAKE_RAIL_H
#include <ninja.h>

struct task;

/* the rail's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_snake_rail_texlists[];

/* the object list's step for SNEAKRAIL. task needs a work. its pos places the
 * rail, and its ang.y turns it.
 *
 * the players collide with a copy of the rail's shape in the collision world,
 * which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies within 34.223 of its pos */
void cave_snake_rail_init(struct task *task);

#endif
