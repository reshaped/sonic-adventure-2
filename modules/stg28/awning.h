/* the awning, AWNING in Pyramid Cave's object list: a solid awning with three
 * fires burning on it */
#ifndef STG28_AWNING_H
#define STG28_AWNING_H
#include <ninja.h>

struct task;

/* the awning's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_awning_texlists[];

/* the object list's step for AWNING. task needs a work. its pos places the
 * awning, and its ang.y turns it. the call sets ang.x and ang.z to 0.
 *
 * the players collide with a copy of the awning's shape in the collision
 * world, which task->extra holds. world_pool_alloc lends the copy, and the
 * call writes through null where the pool has none. the copy collides only
 * while a player lies within 310 of its pos.
 *
 * the call also gives task a collision record of three capsules, the fires,
 * which hurt players. the fires' sound plays at a point 20 above pos. the
 * point lies about ang.y along z from there, ang.y read as a length. the
 * sound plays only while player 0 lies within 120 of the point */
void cave_awning_init(struct task *task);

#endif
