/* the signboards, SB_S and SB_L in Mission Street's object lists. an SB_S
 * board shows one of three pictures */
#ifndef SHARED_SIGNBOARDS_H
#define SHARED_SIGNBOARDS_H
#include <ninja.h>

struct task;

/* both boards' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *signboards_texlists[];

/* the object list's step for SB_S. task needs a work, whose pos places the
 * board and whose ang.y turns it. the low byte of ang.x, modulo 3, picks the
 * picture the board shows */
void signboard_small_init(struct task *task);
/* the object list's step for SB_L, which makes task a model object of
 * shared/common_object.h. task needs a work, whose pos places the board and
 * whose ang.y turns it */
void signboard_large_init(struct task *task);

#endif
