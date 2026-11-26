/* the board, BOARD in Pyramid Cave's object list: a blue or green board under
 * a glow that pulses */
#ifndef STG28_CAVE_BOARD_H
#define STG28_CAVE_BOARD_H
#include <ninja.h>

struct task;

/* the board's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *cave_board_texlists[];

/* the object list's step for BOARD. task needs a work. its pos places the
 * board, and its ang.y turns it. scl.x plus 1 scales it along x and y. the
 * whole part of |scl.z| modulo 2 picks its kind: 0 is blue, and 1 green. the
 * board has no collision record.
 *
 * the glow adds its colors to those drawn before it. it brightens over 5 of
 * every 60 of game_state's steps, and fades over the other 55 */
void cave_board_init(struct task *task);

#endif
