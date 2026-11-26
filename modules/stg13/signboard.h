/* the signboard, SIGNBOARD in City Escape's object list: a board that shows
 * one of five pictures */
#ifndef STG13_SIGNBOARD_H
#define STG13_SIGNBOARD_H
#include <ninja.h>

struct task;

/* the signboard's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *signboard_texlists[];

/* the object list's step for SIGNBOARD. unless placement_try_leave(task)
 * returns TRUE, it gives task the board's own step, draw and destructor. the
 * step only makes the same test. task needs a work, whose pos places the board
 * and whose ang.y turns it. the low byte of ang.x, modulo 5, picks the picture
 * the board shows */
void signboard_init(struct task *task);

#endif
