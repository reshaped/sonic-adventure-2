/* the snake statue, SNAKESTATUE in Pyramid Cave's object list: a solid statue
 * that faces right or left */
#ifndef STG28_SNAKE_STATUE_H
#define STG28_SNAKE_STATUE_H
#include <ninja.h>

struct task;

/* the kinds' texture lists for texture_load_shared, RIGHT's and LEFT's, ended
 * by null */
extern NJS_TEXLIST *cave_snake_statue_texlists[];

/* the object list's step for SNAKESTATUE. task needs a work. its pos places
 * the statue, and its ang.y turns it. the call sets ang.x and ang.z to 0. the
 * whole part of |scl.z| modulo 2 picks its kind: 0 is RIGHT, and 1 LEFT.
 *
 * the players collide with a copy of the kind's shape in the collision world,
 * which task->extra holds. world_pool_alloc lends the copy, and the call
 * writes through null where the pool has none. the copy collides only while a
 * player lies within 113.56 of its pos */
void cave_snake_statue_init(struct task *task);

#endif
