/* the second board, BOARD2 in Wild Canyon's object list: a painted board that
 * the players collide with. it comes bright or dark */
#ifndef SHARED_BOARD2_H
#define SHARED_BOARD2_H
#include <ninja.h>

struct task;

/* the boards' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *board2_texlists[];

/* the object list's step for BOARD2. task needs a work, whose pos places the
 * board. its ang turns the board's draw by njRotateZ, njRotateY and njRotateX,
 * in that order. it also turns the board's shape, in the order the shape's
 * object gives. the board is 1 + scl.x times its size in x and y. the whole
 * part of |scl.z|, modulo 2, picks the kind: BRIGHT or DARK. the board ends
 * where half_try_leave or placement_try_leave returns TRUE.
 *
 * it adds a copy of the shape to the collision world, which world_pool_alloc
 * lends and task->extra keeps. the pool must have one to lend, unchecked. the
 * destructor takes the copy out of the world and back to the pool. the copy
 * collides only while player 0 or 1 is within the model's radius plus 10 of
 * pos */
void board2_init(struct task *task);

#endif
