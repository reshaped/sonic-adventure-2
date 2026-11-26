/* the wall, WALL in Pyramid Cave's object list: a solid wall with an overlay
 * that pulses. it draws see-through while it hides the current camera's
 * player */
#ifndef STG28_WALL_H
#define STG28_WALL_H
#include <ninja.h>

struct task;

/* the wall's texture lists for texture_load_shared, TYPE_A's and TYPE_B's,
 * ended by null */
extern NJS_TEXLIST *cave_wall_texlists[];

/* the object list's step for WALL. task needs a work. its pos places the
 * wall, and its ang.y turns it. the whole part of |scl.x| modulo 2 picks its
 * type: 0 is TYPE_A, and 1 TYPE_B. the whole part of |scl.z| modulo 2 picks
 * its width along x. 0 is 0.8 of the model's, and 1 is 1.0625.
 *
 * the players collide with a copy of the wall's shape in the collision world,
 * which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while
 * a player lies within 113.4 of its pos.
 *
 * the call also gives task a collision record of one box. the box pushes the
 * shapes of every group, and none of theirs pushes it.
 *
 * the current camera's player's work must not be null, unchecked */
void cave_wall_init(struct task *task);

#endif
