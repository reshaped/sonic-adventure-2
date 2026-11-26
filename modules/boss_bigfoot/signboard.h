/* the signboards, SB_S and SIGNBOARDS in the Big Foot stage's object list. an
 * SB_S board shows one of two texture lists, and a SIGNBOARDS board one of
 * three pictures */
#ifndef BOSS_BIGFOOT_SIGNBOARD_H
#define BOSS_BIGFOOT_SIGNBOARD_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* both boards' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *signboard_texlists[];

/* the object list's steps for SB_S and SIGNBOARDS. where
 * placement_try_leave(task) returns TRUE, each does nothing. otherwise each
 * gives task the board's draw, and a step that only calls
 * placement_try_leave.
 *
 * task needs a work, whose pos places the board and whose ang.y turns it.
 * the low bit of ang.x picks the texture list an SB_S board draws with. the
 * low byte of ang.x, modulo 3, picks the picture a SIGNBOARDS board shows */
void signboard_s_init(struct task *task);
void signboards_init(struct task *task);

#endif
