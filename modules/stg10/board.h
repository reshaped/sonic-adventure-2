/* the board, BOARD in Metal Harbor's object list: a board a player rides */
#ifndef STG10_BOARD_H
#define STG10_BOARD_H
#include <ninja.h>

struct task;

/* the board's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_board_texlists[];

/* the object list's step for BOARD. it gives task the board's own step, draw
 * and destructor.
 *
 * task needs a work and a motion record. pos and ang place the board. the low
 * bit of ang.x picks its player, in slot 0 or 1. while that player plays a
 * board animation, the board follows it and plays the animation's own board
 * motion. once the player plays another animation, the board falls on at the
 * player's last speed. only a falling board leaves, where
 * placement_try_leave(task) returns TRUE. while the slot is empty, the step
 * and the draw do nothing */
void harbor_board_init(struct task *task);

#endif
