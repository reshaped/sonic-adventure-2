/* the arrow signs, YAJI and YAJI2 in the stage's object list. each is an
 * arrow's model, turned and scaled. neither puts anything in the collision
 * world */
#ifndef STG03_ARROW_SIGN_H
#define STG03_ARROW_SIGN_H
#include <ninja.h>

struct task;

/* the texture lists of YAJI's sign and YAJI2's, ended by null. the stage
 * passes the array to texture_load_shared among its groups */
extern NJS_TEXLIST *arrow_sign_texlists[];

/* the object list's steps for YAJI and YAJI2. unless
 * placement_try_leave(task) returns TRUE, each gives task the sign's own step,
 * draw and destructor.
 *
 * task needs a work. the sign stands still at pos, turned by ang as
 * njRotateEx turns. along each axis, it scales by scl's field plus 1. the
 * modifier volumes shadow YAJI2's sign, and not YAJI's */
void arrow_sign_init(struct task *task);
void arrow_sign_2_init(struct task *task);

#endif
