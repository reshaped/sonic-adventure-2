/* the block, BLOCK in Pyramid Cave's object list: a solid block, turned and
 * scaled as its work says */
#ifndef STG28_BLOCK_H
#define STG28_BLOCK_H
#include <ninja.h>

struct task;

/* the block's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_block_texlists[];

/* the object list's step for BLOCK. task needs a work. its pos places the
 * block, and its ang turns it by njRotateZ, njRotateX and njRotateY. scl.x,
 * scl.y and scl.z plus 1 scale it along x, y and z. the block draws under
 * DRAW_CONTROL_SHADOW, so the modifier volumes shadow it.
 *
 * the players collide with a copy of the block's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. the copy collides only
 * while a player lies near its pos. near is within 10 plus 200 times the
 * block's largest scale */
void cave_block_init(struct task *task);

#endif
