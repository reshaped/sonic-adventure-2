/* the snake head, SNEAKHEAD in Pyramid Cave's object list: a solid fire pot
 * shaped as a snake's head. a fire burns at either side of it */
#ifndef STG28_SNAKE_HEAD_H
#define STG28_SNAKE_HEAD_H
#include <ninja.h>

struct task;

/* the head's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_snake_head_texlists[];

/* the object list's step for SNEAKHEAD. task needs a work. its pos places the
 * head, and its ang.y turns it. the call sets ang.x and ang.z to 0.
 *
 * the players collide with a copy of the head's shape in the collision world,
 * which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies within 310 of its pos.
 *
 * the call also gives task a collision record of two capsules, the fires,
 * which hurt players. each fire's sound plays from 70 to its side of pos and
 * 15 above. it plays only while player 0 lies within 120 of there. player
 * 0's work must not be null, unchecked */
void cave_snake_head_init(struct task *task);

#endif
