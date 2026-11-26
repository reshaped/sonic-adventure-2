/* the board, BOARD in Wild Canyon's object list: a sign on a post, in one
 * of three kinds */
#ifndef SHARED_BOARD_H
#define SHARED_BOARD_H
#include <ninja.h>

struct task;

/* the boards' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *board_texlists[];

/* the object list's step for BOARD. task needs a work, whose pos places the
 * board. its ang turns the board by njRotateZ, njRotateX and njRotateY, in
 * that order. an odd (Sint32)scl.y then tips the board back a quarter turn
 * about x. the board draws 1 + scl.x times its size in x and y. the whole part
 * of |scl.z|, modulo 3, picks the kind: UP BRIGHT, UP DARK or LOW. the board
 * ends where half_try_leave or placement_try_leave returns TRUE */
void board_init(struct task *task);

#endif
